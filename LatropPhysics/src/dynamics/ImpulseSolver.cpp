#include "dynamics/ImpulseSolver.h"
#include "dynamics/RigidBody.h"

using namespace LP;

void ImpulseSolver::solve(const std::vector<Collision>& collisions, float deltaTime)
{
    for (const Collision& manifold : collisions) 
    {
        // Replaces non dynamic objects with default values.

        RigidBody* aBody = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* bBody = dynamic_cast<RigidBody*>(manifold.bodyB);

        glm::vec3 aVel = aBody ? aBody->linearVelocity : glm::vec3(0.0f);
        glm::vec3 bVel = bBody ? bBody->linearVelocity : glm::vec3(0.0f);
        glm::vec3 rVel = bVel - aVel;
        float nSpd = glm::dot(rVel, manifold.points.normal);

        float aInvMass = aBody->getInvMass();
        float bInvMass = bBody->getInvMass();

        // Impulse

        // This is important for convergence
        // a negitive impulse would drive the objects closer together
        if (nSpd >= 0)
            continue;

        float e = (aBody ? aBody->material.restitution : 1.0f)
                * (bBody ? bBody->material.restitution : 1.0f);

        float j = -(1.0f + e) * nSpd / (aInvMass + bInvMass);

        glm::vec3 impluse = j * manifold.points.normal;

        if (aBody ? aBody->isSimulated() : false) 
        {
            aVel -= impluse * aInvMass;
        }

        if (bBody ? bBody->isSimulated() : false) 
        {
            bVel += impluse * bInvMass;
        }

        // Friction

        rVel = bVel - aVel;
        nSpd = glm::dot(rVel, manifold.points.normal);

        glm::vec3 tangent = rVel - nSpd * manifold.points.normal;

        if (glm::length(tangent) > 0.0001f) 
        { // safe normalize
            tangent = glm::normalize(tangent);
        }

        float fVel = glm::dot(rVel, tangent);

        float aSF = aBody ? aBody->material.staticFriction  : 0.0f;
        float bSF = bBody ? bBody->material.staticFriction  : 0.0f;
        float aDF = aBody ? aBody->material.dynamicFriction : 0.0f;
        float bDF = bBody ? bBody->material.dynamicFriction : 0.0f;
        float mu  = (float)glm::vec2(aSF, bSF).length();

        float f  = -fVel / (aInvMass + bInvMass);

        glm::vec3 friction;
        if (abs(f) < j * mu) 
        {
            friction = f * tangent;
        }
        else 
        {
            mu = glm::length(glm::vec2(aDF, bDF));
            friction = -j * tangent * mu;
        }

        if (aBody ? aBody->isSimulated() : false) 
        {
            aBody->linearVelocity = aVel - friction * aInvMass;
        }

        if (bBody ? bBody->isSimulated() : false) 
        {
            bBody->linearVelocity = bVel + friction * bInvMass;

            // Temporary logic to allow stairs-like behaviour(and climbing walls)
            // Allow stepping up small vertical surfaces instead of treating them as collisions
            const float maxStepHeight = 0.01f;
            if (glm::abs(manifold.points.normal.y) < 0.5f && manifold.points.depth < maxStepHeight)
            {
                bBody->transform.position.y += (maxStepHeight - manifold.points.depth);
                continue; // Skip normal impulse resolution
            }
        }
    }
}