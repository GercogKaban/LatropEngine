#include "dynamics/RotationaImpulseSolver.h"
#include "dynamics/RigidBody.h"

using namespace LP;

void RotationaImpulseSolver::solve(const std::vector<CollisionManifold>& collisions, float deltaTime)
{
    for (const CollisionManifold& manifold : collisions) 
    {
        RigidBody* aBody = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* bBody = dynamic_cast<RigidBody*>(manifold.bodyB);

        if (!(aBody && bBody)) continue;

        Material combinedMaterial = aBody->material.combinedWith(bBody->material);

        float e = combinedMaterial.restitution;

        std::vector<glm::vec3> impulses;
        std::vector<glm::vec3> rAs;
        std::vector<glm::vec3> rBs;

        for (uint8_t i = 0; i < manifold.contactsCount; ++i) 
        {
            const ContactPoint& contact = manifold.contactPoints[i];

            glm::vec3 rA = contact.location - aBody->transform.position;
            glm::vec3 rB = contact.location - bBody->transform.position;

            rAs.push_back(rA);
            rBs.push_back(rB);

            glm::vec3 rAPerp = glm::cross(aBody->angularVelocity, rA);
            glm::vec3 rBPerp = glm::cross(bBody->angularVelocity, rB);

            // glm::vec3 angularLinearVelA = rAPerp * aBody->angularVelocity;
            // glm::vec3 angularLinearVelB = rBPerp * bBody->angularVelocity;

            glm::vec3 aVel = aBody->linearVelocity;
            glm::vec3 bVel = bBody->linearVelocity;

            glm::vec3 rVel = (bVel + rBPerp) - (aVel + rAPerp);
            float contactVelocityMagnitude = glm::dot(rVel, manifold.normal);

            // This is important for convergence
            // a negative impulse would drive the objects closer together
            if (contactVelocityMagnitude >= 0) continue;

            float aInvMass = aBody->getInvMass();
            float bInvMass = bBody->getInvMass();

            // Impulse

            float raPerpDotN = glm::dot(rAPerp, manifold.normal);
            float rbPerpDotN = glm::dot(rBPerp, manifold.normal);

            float denom = aInvMass + bInvMass 
            + glm::pow(raPerpDotN, 2.0f) * aBody->invInertiaTensorLocal[0][0]
            + glm::pow(rbPerpDotN, 2.0f) * bBody->invInertiaTensorLocal[0][0];

            float j = -(1.0f + e) * contactVelocityMagnitude / denom;
            j /= float(manifold.contactsCount);

            glm::vec3 impulse = j * manifold.normal;
            impulses.push_back(impulse);
        }

        for (u_int8_t i = 0; i < manifold.contactsCount; ++i)
        {
            if (i >= impulses.size()) continue;

            glm::vec3 impulse = impulses[i];

            if (aBody->isSimulated()) 
            {
                aBody->linearVelocity -= impulse * aBody->getInvMass();
                aBody->angularVelocity -= glm::cross(rAs[i], impulse) * aBody->invInertiaTensorLocal[0][0];
            }
            if (bBody->isSimulated()) 
            {
                bBody->linearVelocity += impulse * bBody->getInvMass();
                bBody->angularVelocity += glm::cross(rBs[i], impulse) * bBody->invInertiaTensorLocal[0][0];
            }
        }

        // Friction

        // rVel = bVel - aVel;
        // contactVelocityMagnitude = glm::dot(rVel, manifold.normal);

        // glm::vec3 tangent = rVel - contactVelocityMagnitude * manifold.normal;
        // glm::vec3 friction = { 0.0f, 0.0f, 0.0f };

        // if (glm::length(tangent) > 0.0001f) 
        // { 
        //     tangent = glm::normalize(tangent);

        //     float fVel = glm::dot(rVel, tangent);
        //     float f    = -fVel / (aInvMass + bInvMass);

        //     float staticMu  = combinedMaterial.staticFriction;
        //     float dynamicMu = combinedMaterial.dynamicFriction;

        //     if (glm::abs(f) < j * staticMu) friction = f * tangent;
        //     else                            friction = -j * dynamicMu * tangent;
        // }

        // // Final Calculations

        // if (aBody->isSimulated()) 
        // {
        //     aBody->linearVelocity = aVel - friction * aInvMass;
        // }

        // if (bBody->isSimulated()) 
        // {
        //     bBody->linearVelocity = bVel + friction * bInvMass;
        // }
    }
}