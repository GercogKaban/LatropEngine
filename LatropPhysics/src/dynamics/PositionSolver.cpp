#include "dynamics/PositionSolver.h"
#include "dynamics/RigidBody.h"

using namespace LP;

void PositionSolver::solve(const std::vector<Collision>& collisions, float deltaTime)
{
    for (const Collision& manifold : collisions) 
    {
        RigidBody* aBody = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* bBody = dynamic_cast<RigidBody*>(manifold.bodyB);
        
        float aInvMass = aBody->getInvMass();
        float bInvMass = bBody->getInvMass();

        // === Positional Correction ===
        const float percent = 0.2f;  // usually 20% to 80%
        const float slop = 0.01f;    // penetration allowance
        float correctionMagnitude = glm::max(manifold.points.depth - slop, 0.0f) / (aInvMass + bInvMass) * percent;
        glm::vec3 correction = correctionMagnitude * manifold.points.normal;

        if (aBody && aBody->isSimulated())
        {
            aBody->transform.position -= aInvMass * correction * (bBody && bBody->isSimulated() ? 1.0f : 2.0f);
        } 

        if (bBody && bBody->isSimulated()) 
        {
            // Temporary logic to allow stairs-like behaviour(and climbing walls)
            // Check if this is a potential stair
            if (glm::abs(manifold.points.normal.y) < 0.5f && manifold.points.depth < slop)
            {
                // Step up instead of resolving normally
                bBody->transform.position.y += slop - manifold.points.depth;
                continue; // skip regular resolution
            }

            bBody->transform.position += bInvMass * correction * (aBody && aBody->isSimulated() ? 1.0f : 2.0f);
        }
    }

    // for (Collision& manifold : collisions) {
    //     RigidBody* aBody = manifold.bodyA;
    //     RigidBody* bBody = manifold.bodyB;

    //     float aStatic = (float)(int)!aBody->m_isSimulated;
    //     float bStatic = (float)(int)!bBody->m_isSimulated;

    //     glm::vec3 resolution = manifold.Normal 
    //         * manifold.Depth 
    //         / iw::max<scalar>(1, aStatic + bStatic);

    //     aBody->Transform.Position -= resolution * scalar(1 - aStatic);
    //     bBody->Transform.Position += resolution * scalar(1 - bStatic);
    // }
}