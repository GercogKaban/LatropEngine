#include "dynamics/PositionSolver.h"
#include "dynamics/RigidBody.h"

using namespace LP;

void PositionSolver::solve(const std::vector<CollisionManifold>& collisions, float deltaTime)
{
    for (const CollisionManifold& manifold : collisions) 
    {
        RigidBody* aBody = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* bBody = dynamic_cast<RigidBody*>(manifold.bodyB);
        
        float aInvMass = aBody->getInvMass();
        float bInvMass = bBody->getInvMass();

        // === Positional Correction ===
        const float percent = 0.2f;  // usually 20% to 80%
        const float slop = 0.01f;    // penetration allowance
        float correctionMagnitude = glm::max(manifold.depth - slop, 0.0f) / (aInvMass + bInvMass) * percent;
        glm::vec3 correction = correctionMagnitude * manifold.normal;

        if (aBody && aBody->isSimulated())
        {
            aBody->transform.position -= aInvMass * correction * (bBody && bBody->isSimulated() ? 1.0f : 2.0f);
        } 

        if (bBody && bBody->isSimulated()) 
        {
            bBody->transform.position += bInvMass * correction * (aBody && aBody->isSimulated() ? 1.0f : 2.0f);
        }
    }
}