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

        if (!(aBody && bBody))
        {
            continue;
        }

        Material combinedMaterial = aBody->material.combinedWith(bBody->material);

        glm::vec3 aVel = aBody->linearVelocity;
        glm::vec3 bVel = bBody->linearVelocity;
        glm::vec3 rVel = bVel - aVel;
        float nSpd = glm::dot(rVel, manifold.points.normal);

        float aInvMass = aBody->getInvMass();
        float bInvMass = bBody->getInvMass();

        // Impulse

        // This is important for convergence
        // a negitive impulse would drive the objects closer together
        if (nSpd >= 0)
            continue;

        float e = combinedMaterial.restitution;
        float j = -(1.0f + e) * nSpd / (aInvMass + bInvMass);

        glm::vec3 impluse = j * manifold.points.normal;

        if (aBody->isSimulated()) 
        {
            aVel -= impluse * aInvMass;
        }

        if (bBody->isSimulated()) 
        {
            bVel += impluse * bInvMass;
        }

        // Friction

        rVel = bVel - aVel;
        nSpd = glm::dot(rVel, manifold.points.normal);

        glm::vec3 tangent = rVel - nSpd * manifold.points.normal;

        if (glm::length(tangent) > 0.0001f) 
        { 
            tangent = glm::normalize(tangent);
        }

        float fVel = glm::dot(rVel, tangent);
        float f    = -fVel / (aInvMass + bInvMass);

        float staticMu  = combinedMaterial.staticFriction;
        float dynamicMu = combinedMaterial.dynamicFriction;

        glm::vec3 friction;
        
        if (glm::abs(f) < j * staticMu) 
        {
            friction = f * tangent;
        }
        else 
        {
            friction = -j * dynamicMu * tangent;
        }

        if (aBody->isSimulated()) 
        {
            aBody->linearVelocity = aVel - friction * aInvMass;
        }

        if (bBody->isSimulated()) 
        {
            bBody->linearVelocity = bVel + friction * bInvMass;
        }
    }
}