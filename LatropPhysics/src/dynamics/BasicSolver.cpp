#include "dynamics/BasicSolver.h"
#include "dynamics/RigidBody.h"

using namespace LatropPhysics;

void BasicSolver::solve(std::vector<Collision>& collisions, float deltaTime)
{

    for (Collision& manifold : collisions) {
        CollisionBody* aBody = manifold.bodyA;
        CollisionBody* bBody = manifold.bodyB;

        RigidBody* a = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* b = dynamic_cast<RigidBody*>(manifold.bodyB);

        float aStatic = a ? 0 : 1;
        float bStatic = b ? 0 : 1;

        glm::vec3 resolution = manifold.points.normal 
            * manifold.points.depth 
            / glm::max<float>(1, aStatic + bStatic);

        aBody->transform->position -= resolution * float(1 - aStatic);
        bBody->transform->position += resolution * float(1 - bStatic);
    }

    // for (Collision& manifold : collisions) {
    //     // Replaces non dynamic objects with default values.

    //     RigidBody* aBody = dynamic_cast<RigidBody*>(manifold.bodyA);
    //     RigidBody* bBody = dynamic_cast<RigidBody*>(manifold.bodyB);

    //     glm::vec3 aVel = aBody ? aBody->m_velocity : glm::vec3(0.0f);
    //     glm::vec3 bVel = bBody ? bBody->m_velocity : glm::vec3(0.0f);
    //     glm::vec3 rVel = bVel - aVel;
    //     float nSpd = glm::dot(rVel, manifold.points.normal);

    //     float aInvMass = aBody ? 1 / aBody->m_mass : 1.0f;
    //     float bInvMass = bBody ? 1 / bBody->m_mass : 1.0f;

    //     // Impulse

    //     // This is important for convergence
    //     // a negitive impulse would drive the objects closer together
    //     if (nSpd >= 0)
    //         continue;

    //     float e = 1.0f;//(aBody ? aBody->Restitution : 1.0f)
    //              //* (bBody ? bBody->Restitution : 1.0f);

    //     float j = -(1.0f + e) * nSpd / (aInvMass + bInvMass);

    //     glm::vec3 impluse = j * manifold.points.normal;

    //     if (aBody/* ? aBody->IsSimulated : false*/) {
    //         aVel -= impluse * aInvMass;
    //     }

    //     if (bBody/* ? bBody->IsSimulated : false*/) {
    //         bVel += impluse * bInvMass;
    //     }

    //     // Friction

    //     rVel = bVel - aVel;
    //     nSpd = glm::dot(rVel, manifold.points.normal);

    //     glm::vec3 tangent = rVel - nSpd * manifold.points.normal;

    //     if (glm::length(tangent) > 0.0001f) { // safe normalize
    //         tangent = glm::normalize(tangent);
    //     }

    //     float fVel = glm::dot(rVel, tangent);

    //     float aSF = /*aBody ? aBody->StaticFriction  : */0.0f;
    //     float bSF = /*bBody ? bBody->StaticFriction  : */0.0f;
    //     float aDF = /*aBody ? aBody->DynamicFriction : */0.0f;
    //     float bDF = /*bBody ? bBody->DynamicFriction : */0.0f;
    //     float mu  = (float)glm::vec2(aSF, bSF).length();

    //     float f  = -fVel / (aInvMass + bInvMass);

    //     glm::vec3 friction;
    //     if (abs(f) < j * mu) {
    //         friction = f * tangent;
    //     }

    //     else {
    //         mu = glm::length(glm::vec2(aDF, bDF));
    //         friction = -j * tangent * mu;
    //     }

    //     if (aBody/* aBody->IsSimulated : false*/) {
    //         aBody->m_velocity = aVel - friction * aInvMass;
    //     }

    //     if (bBody/* ? bBody->IsSimulated : false*/) {
    //         bBody->m_velocity = bVel + friction * bInvMass;
    //     }
    // }


    // for (Collision& collision : collisions) {
    //     if (!collision.points.hasCollision) continue;

    //     RigidBody* bodyA = dynamic_cast<RigidBody*>(collision.bodyA);
    //     RigidBody* bodyB = dynamic_cast<RigidBody*>(collision.bodyB);

    //     glm::vec3 correction = collision.points.normal * collision.points.depth * 0.5f;

    //     // Position correction
    //     if (bodyA) bodyA->transform->position -= correction;
    //     if (bodyB) bodyB->transform->position += correction;

    //     // Velocity adjustment
    //     glm::vec3 velocityA, velocityB;

    //     if (bodyA) velocityA = bodyA->m_velocity;
    //     else       velocityA = { 0.0f, 0.0f, 0.0f };

    //     if (bodyB) velocityB = bodyB->m_velocity;
    //     else       velocityB = { 0.0f, 0.0f, 0.0f };

    //     glm::vec3 relativeVelocity = velocityB - velocityA;
    //     float normalVelocity = glm::dot(relativeVelocity, collision.points.normal);
        
    //     if (normalVelocity < 0.0f) {
    //         glm::vec3 impulse = -normalVelocity * collision.points.normal;
    //         if (bodyA) bodyA->m_velocity -= impulse;
    //         if (bodyB) bodyB->m_velocity += impulse;
    //     }

    //     // Reset forces if stuck
    //     if (bodyA) 
    //     {
    //         bodyA->m_force -= glm::dot(bodyA->m_force, collision.points.normal) * collision.points.normal;
    //     }

    //     if (bodyB)
    //     { 
    //         bodyB->m_force -= glm::dot(bodyB->m_force, collision.points.normal) * collision.points.normal;
    //     }
    // }
}