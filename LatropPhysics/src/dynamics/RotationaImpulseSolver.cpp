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
        float staticMu  = combinedMaterial.staticFriction;
        float dynamicMu = combinedMaterial.dynamicFriction;

        float aInvMass = aBody->getInvMass();
        float bInvMass = bBody->getInvMass();

        std::vector<glm::vec3> impulses;
        std::vector<float> jValues;
        std::vector<glm::vec3> rAs;
        std::vector<glm::vec3> rBs;

        // Impulses

        for (uint8_t i = 0; i < manifold.contactsCount; ++i) 
        {
            const ContactPoint& contact = manifold.contactPoints[i];

            glm::vec3 rA = contact.location - aBody->transform.position;
            glm::vec3 rB = contact.location - bBody->transform.position;

            rAs.push_back(rA);
            rBs.push_back(rB);

            glm::vec3 angularLinearVelA = glm::cross(aBody->angularVelocity, rA);
            glm::vec3 angularLinearVelB = glm::cross(bBody->angularVelocity, rB);

            glm::vec3 aVel = aBody->linearVelocity;
            glm::vec3 bVel = bBody->linearVelocity;

            glm::vec3 rVel = (bVel + angularLinearVelB) - (aVel + angularLinearVelA);
            float contactVelocityMagnitude = glm::dot(rVel, manifold.normal);

            // This is important for convergence
            // a negative impulse would drive the objects closer together
            if (contactVelocityMagnitude >= 0) continue;

            // Normal Impulse

            float raPerpDotN = glm::dot(angularLinearVelA, manifold.normal);
            float rbPerpDotN = glm::dot(angularLinearVelB, manifold.normal);
            glm::vec3 n = manifold.normal;

            float numer = -(1.0f + e) * contactVelocityMagnitude;
            float denom1 = glm::dot(n, n) * (aInvMass + bInvMass);
            glm::vec3 denom2_1 = glm::cross(aBody->getInvInertiaTensor() * glm::cross(angularLinearVelA, n), angularLinearVelA)
                               + glm::cross(bBody->getInvInertiaTensor() * glm::cross(angularLinearVelB, n), angularLinearVelB);

            float denom2 = glm::dot(denom2_1, n);

            float denom = denom1 + denom2;

            // float denom = aInvMass + bInvMass 
            // + glm::dot(manifold.normal, glm::cross(rAs[i], manifold.normal) * aBody->getInvInertiaTensor())
            // + glm::dot(manifold.normal, glm::cross(rBs[i], manifold.normal) * bBody->getInvInertiaTensor());

            // float denom = aInvMass + bInvMass 
            // + glm::pow(raPerpDotN, 2.0f) * aBody->getInvInertiaTensor()[0][0]
            // + glm::pow(rbPerpDotN, 2.0f) * bBody->getInvInertiaTensor()[0][0];

            float j = numer / denom;
            j /= float(manifold.contactsCount);

            glm::vec3 impulse = j * manifold.normal;
            impulses.push_back(impulse);
            jValues.push_back(j);
        }

        for (u_int8_t i = 0; i < manifold.contactsCount; ++i)
        {
            if (i >= impulses.size()) continue;

            glm::vec3 impulse = impulses[i];

            if (aBody->isSimulated()) 
            {
                aBody->linearVelocity -= impulse * aBody->getInvMass();
                aBody->angularVelocity -= aBody->getInvInertiaTensor() * glm::cross(rAs[i], impulse);
            }
            if (bBody->isSimulated()) 
            {
                bBody->linearVelocity += impulse * bBody->getInvMass();
                bBody->angularVelocity += bBody->getInvInertiaTensor() * glm::cross(rBs[i], impulse);
            }
        }

        // Friction
        impulses.clear();

        for (uint8_t i = 0; i < manifold.contactsCount; ++i) 
        {
            const ContactPoint& contact = manifold.contactPoints[i];

            glm::vec3 rA = contact.location - aBody->transform.position;
            glm::vec3 rB = contact.location - bBody->transform.position;

            rAs.push_back(rA);
            rBs.push_back(rB);

            glm::vec3 angularLinearVelA = glm::cross(aBody->angularVelocity, rA);
            glm::vec3 angularLinearVelB = glm::cross(bBody->angularVelocity, rB);

            glm::vec3 aVel = aBody->linearVelocity;
            glm::vec3 bVel = bBody->linearVelocity;

            glm::vec3 rVel = (bVel + angularLinearVelB) - (aVel + angularLinearVelA);
            glm::vec3 tangent = rVel - glm::dot(rVel, manifold.normal) * manifold.normal;

            if (glm::length(tangent) < 0.0001f) continue;

            tangent = glm::normalize(tangent);

            // Tangent Impulse

            float raPerpDotT = glm::dot(angularLinearVelA, tangent);
            float rbPerpDotT = glm::dot(angularLinearVelB, tangent);

            glm::vec3 n = tangent;
            float contactVelocityMagnitude = glm::dot(rVel, tangent);

            float numer = -contactVelocityMagnitude;
            float denom1 = glm::dot(n, n) * (aInvMass + bInvMass);
            glm::vec3 denom2_1 = glm::cross(aBody->getInvInertiaTensor() * glm::cross(angularLinearVelA, n), angularLinearVelA)
                               + glm::cross(bBody->getInvInertiaTensor() * glm::cross(angularLinearVelB, n), angularLinearVelB);

            float denom2 = glm::dot(denom2_1, n);

            float denom = denom1 + denom2;

            // float denom = aInvMass + bInvMass 
            // + glm::pow(raPerpDotN, 2.0f) * aBody->getInvInertiaTensor()[0][0]
            // + glm::pow(rbPerpDotN, 2.0f) * bBody->getInvInertiaTensor()[0][0];

            float jt = numer / denom;
            jt /= float(manifold.contactsCount);

            glm::vec3 frictionImpulse;

            float j = jValues.size() > i ? jValues[i] : 0.0f;

            if (glm::abs(jt) < j * staticMu) frictionImpulse = jt * tangent;
            else                             frictionImpulse = -j * tangent * dynamicMu;

            impulses.push_back(frictionImpulse);
        }

        for (u_int8_t i = 0; i < manifold.contactsCount; ++i)
        {
            if (i >= impulses.size()) continue;

            glm::vec3 impulse = impulses[i];

            if (aBody->isSimulated()) 
            {
                aBody->linearVelocity -= impulse * aBody->getInvMass();
                aBody->angularVelocity -= aBody->getInvInertiaTensor() * glm::cross(rAs[i], impulse);
            }
            if (bBody->isSimulated()) 
            {
                bBody->linearVelocity += impulse * bBody->getInvMass();
                bBody->angularVelocity += bBody->getInvInertiaTensor() * glm::cross(rBs[i], impulse);
            }
        }
    }
}