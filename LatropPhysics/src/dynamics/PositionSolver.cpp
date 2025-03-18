#include "dynamics/PositionSolver.h"
#include "dynamics/RigidBody.h"

using namespace LatropPhysics;

void PositionSolver::solve(const std::vector<Collision>& collisions, float deltaTime)
{
    for (const Collision& manifold : collisions) 
    {
        RigidBody* aBody = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* bBody = dynamic_cast<RigidBody*>(manifold.bodyB);
        
        float aInvMass = aBody ? 1 / aBody->m_mass : 1.0f;
        float bInvMass = bBody ? 1 / bBody->m_mass : 1.0f;

        // === Positional Correction ===
        const float percent = 0.2f;  // usually 20% to 80%
        const float slop = 0.01f;    // penetration allowance
        float correctionMagnitude = glm::max(manifold.points.depth - slop, 0.0f) / (aInvMass + bInvMass) * percent;
        glm::vec3 correction = correctionMagnitude * manifold.points.normal;

        if (aBody && aBody->m_isSimulated) aBody->transform.position -= aInvMass * correction;
        if (bBody && bBody->m_isSimulated) bBody->transform.position += bInvMass * correction;
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