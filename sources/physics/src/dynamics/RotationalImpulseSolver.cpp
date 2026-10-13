#include "dynamics/RotationalImpulseSolver.h"
#include "dynamics/RigidBody.h"

#include <algorithm>
#include <cmath>

using namespace LP;

namespace
{
    /// Relative approach speeds below this value are treated as resting contact
    /// and never bounce. Without it the tiny velocity gravity adds every step
    /// is "reflected" by the restitution term, and the body hops forever.
    constexpr float restitutionThreshold = 0.5f;

    /// Tangential speed below which static friction is used.
    constexpr float staticFrictionSpeed = 0.01f;

    /// Four coplanar corner contacts on a rigid body are nearly redundant, which
    /// makes Gauss-Seidel converge slowly. With ~8 iterations a perfectly elastic
    /// landing leaves a small residual spin each bounce, and since nothing damps
    /// it, it accumulates until the body tumbles.
    constexpr int iterations = 20;

    struct Contact
    {
        glm::vec3 rA;
        glm::vec3 rB;

        glm::vec3 tangents[2];

        float normalMass;
        float tangentMass[2];

        /// Target normal speed after the solve: the restitution term, or for a
        /// contact that is still separated the speed that closes the gap exactly.
        float targetSpeed;
        bool useStaticFriction;

        float normalImpulse = 0.0f;
        float tangentImpulse[2] = { 0.0f, 0.0f };
    };

    struct Constraint
    {
        RigidBody* a;
        RigidBody* b;
        glm::vec3 normal;
        float staticMu;
        float dynamicMu;
        std::vector<Contact> contacts;
    };

    inline glm::vec3 pointVelocity(const RigidBody* body, const glm::vec3& r)
    {
        return body->linearVelocity + glm::cross(body->angularVelocity, r);
    }

    /// 1 / (invMassA + invMassB + (rA x d)^T IA^-1 (rA x d) + (rB x d)^T IB^-1 (rB x d))
    inline float effectiveMass(
        const RigidBody* a, const RigidBody* b,
        const glm::vec3& rA, const glm::vec3& rB,
        const glm::mat3& invIA, const glm::mat3& invIB,
        const glm::vec3& direction
    ) {
        const glm::vec3 raXd = glm::cross(rA, direction);
        const glm::vec3 rbXd = glm::cross(rB, direction);

        const float k = a->getInvMass() + b->getInvMass()
                      + glm::dot(raXd, invIA * raXd)
                      + glm::dot(rbXd, invIB * rbXd);

        return k > 0.0f ? 1.0f / k : 0.0f;
    }

    inline void applyImpulse(
        RigidBody* a, RigidBody* b,
        const Contact& contact, const glm::vec3& impulse
    ) {
        if (a->isSimulated())
        {
            a->linearVelocity -= impulse * a->getInvMass();
            a->angularVelocity -= a->getInvInertiaTensor() * glm::cross(contact.rA, impulse);
        }

        if (b->isSimulated())
        {
            b->linearVelocity += impulse * b->getInvMass();
            b->angularVelocity += b->getInvInertiaTensor() * glm::cross(contact.rB, impulse);
        }
    }
}

void RotationalImpulseSolver::solve(const std::vector<CollisionManifold>& collisions, float deltaTime)
{
    std::vector<Constraint> constraints;
    constraints.reserve(collisions.size());

    // MARK: Prepare

    for (const CollisionManifold& manifold : collisions)
    {
        RigidBody* a = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* b = dynamic_cast<RigidBody*>(manifold.bodyB);

        if (!(a && b)) continue;
        if (!a->isSimulated() && !b->isSimulated()) continue;

        const Material material = a->material.combinedWith(b->material);

        Constraint constraint;
        constraint.a = a;
        constraint.b = b;
        constraint.normal = manifold.normal;
        constraint.staticMu = material.staticFriction;
        constraint.dynamicMu = material.dynamicFriction;

        // The inertia tensor only changes with orientation, which is constant
        // during the solve, so it is evaluated once per body.
        const glm::mat3 invIA = a->getInvInertiaTensor();
        const glm::mat3 invIB = b->getInvInertiaTensor();

        // Two orthogonal tangents that do not depend on the current velocity,
        // otherwise the friction direction flips back and forth at rest.
        const glm::vec3& n = manifold.normal;
        const glm::vec3 helper = glm::abs(n.x) < 0.9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
        const glm::vec3 t0 = glm::normalize(glm::cross(n, helper));
        const glm::vec3 t1 = glm::cross(n, t0);

        for (uint32_t i = 0; i < manifold.contactsCount; ++i)
        {
            Contact contact;
            const glm::vec3& location = manifold.contactPoints[i];

            contact.rA = location - a->transform.position;
            contact.rB = location - b->transform.position;
            contact.tangents[0] = t0;
            contact.tangents[1] = t1;

            contact.normalMass = effectiveMass(a, b, contact.rA, contact.rB, invIA, invIB, n);
            contact.tangentMass[0] = effectiveMass(a, b, contact.rA, contact.rB, invIA, invIB, t0);
            contact.tangentMass[1] = effectiveMass(a, b, contact.rA, contact.rB, invIA, invIB, t1);

            const glm::vec3 relativeVelocity = pointVelocity(b, contact.rB) - pointVelocity(a, contact.rA);
            const float approachSpeed = glm::dot(relativeVelocity, n);

            const float separation = std::max(-manifold.penetrations[i], 0.0f);

            const float bounceSpeed = approachSpeed < -restitutionThreshold
                ? -material.restitution * approachSpeed
                : 0.0f;

            // A point that reaches the surface during this step is part of the
            // same impact as its neighbours. Giving it a different target than
            // the points already touching (keep approaching vs. bounce back)
            // would put a net torque on a flat landing.
            const bool reachesSurfaceThisStep = deltaTime > 0.0f && separation <= -approachSpeed * deltaTime;

            // Speculative contact: a point that is still further away may keep
            // approaching, but only as fast as it takes to close the gap in
            // this step. Treating it as touching would make the body stick.
            contact.targetSpeed = (separation > 0.0f && !reachesSurfaceThisStep && deltaTime > 0.0f)
                ? -separation / deltaTime
                : bounceSpeed;

            const glm::vec3 slide = relativeVelocity - approachSpeed * n;
            contact.useStaticFriction = glm::length(slide) < staticFrictionSpeed;

            constraint.contacts.push_back(contact);
        }

        constraints.push_back(std::move(constraint));
    }

    // MARK: Iterate
    // Sequential impulses: every contact sees the velocities left by the
    // previous one, and the accumulated impulse is clamped instead of each
    // individual increment so a contact can take back what it over-applied.

    for (int iteration = 0; iteration < iterations; ++iteration)
    {
        for (Constraint& constraint : constraints)
        {
            RigidBody* a = constraint.a;
            RigidBody* b = constraint.b;

            // Friction first, so the non-penetration constraint has the last word.
            for (Contact& contact : constraint.contacts)
            {
                const float mu = contact.useStaticFriction ? constraint.staticMu : constraint.dynamicMu;
                const float maxFriction = mu * contact.normalImpulse;

                for (int t = 0; t < 2; ++t)
                {
                    const glm::vec3 relativeVelocity = pointVelocity(b, contact.rB) - pointVelocity(a, contact.rA);
                    const float slideSpeed = glm::dot(relativeVelocity, contact.tangents[t]);

                    float lambda = -slideSpeed * contact.tangentMass[t];

                    const float previous = contact.tangentImpulse[t];
                    contact.tangentImpulse[t] = std::clamp(previous + lambda, -maxFriction, maxFriction);
                    lambda = contact.tangentImpulse[t] - previous;

                    applyImpulse(a, b, contact, lambda * contact.tangents[t]);
                }
            }

            for (Contact& contact : constraint.contacts)
            {
                const glm::vec3 relativeVelocity = pointVelocity(b, contact.rB) - pointVelocity(a, contact.rA);
                const float approachSpeed = glm::dot(relativeVelocity, constraint.normal);

                float lambda = (contact.targetSpeed - approachSpeed) * contact.normalMass;

                // Accumulated impulse may never pull the bodies together.
                const float previous = contact.normalImpulse;
                contact.normalImpulse = std::max(previous + lambda, 0.0f);
                lambda = contact.normalImpulse - previous;

                applyImpulse(a, b, contact, lambda * constraint.normal);
            }
        }
    }
}
