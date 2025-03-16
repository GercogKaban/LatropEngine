#include "dynamics/BasicSolver.h"
#include "dynamics/RigidBody.h"

using namespace LatropPhysics;

void BasicSolver::solve(std::vector<Collision>& collisions, float deltaTime)
{
    for (Collision& manifold : collisions) 
    {
        // Replaces non dynamic objects with default values.

        RigidBody* aBody = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* bBody = dynamic_cast<RigidBody*>(manifold.bodyB);

        glm::vec3 aVel = aBody ? aBody->m_velocity : glm::vec3(0.0f);
        glm::vec3 bVel = bBody ? bBody->m_velocity : glm::vec3(0.0f);
        glm::vec3 rVel = bVel - aVel;
        float nSpd = glm::dot(rVel, manifold.points.normal);

        float aInvMass = aBody ? 1 / aBody->m_mass : 1.0f;
        float bInvMass = bBody ? 1 / bBody->m_mass : 1.0f;

        // Impulse

        // This is important for convergence
        // a negitive impulse would drive the objects closer together
        if (nSpd >= 0)
            continue;

        float e = (aBody ? aBody->m_restitution : 1.0f)
                * (bBody ? bBody->m_restitution : 1.0f);

        float j = -(1.0f + e) * nSpd / (aInvMass + bInvMass);

        glm::vec3 impluse = j * manifold.points.normal;

        if (aBody/* ? aBody->IsSimulated : false*/) {
            aVel -= impluse * aInvMass;
        }

        if (bBody/* ? bBody->IsSimulated : false*/) {
            bVel += impluse * bInvMass;
        }

        // Friction

        rVel = bVel - aVel;
        nSpd = glm::dot(rVel, manifold.points.normal);

        glm::vec3 tangent = rVel - nSpd * manifold.points.normal;

        if (glm::length(tangent) > 0.0001f) { // safe normalize
            tangent = glm::normalize(tangent);
        }

        float fVel = glm::dot(rVel, tangent);

        float aSF = aBody ? aBody->m_staticFriction  : 0.0f;
        float bSF = bBody ? bBody->m_staticFriction  : 0.0f;
        float aDF = aBody ? aBody->m_dynamicFriction : 0.0f;
        float bDF = bBody ? bBody->m_dynamicFriction : 0.0f;
        float mu  = (float)glm::vec2(aSF, bSF).length();

        float f  = -fVel / (aInvMass + bInvMass);

        glm::vec3 friction;
        if (abs(f) < j * mu) {
            friction = f * tangent;
        }

        else {
            mu = glm::length(glm::vec2(aDF, bDF));
            friction = -j * tangent * mu;
        }

        if (aBody/* aBody->IsSimulated : false*/) {
            aBody->m_velocity = aVel - friction * aInvMass;
        }

        if (bBody/* ? bBody->IsSimulated : false*/) {
            bBody->m_velocity = bVel + friction * bInvMass;
        }
    }
}