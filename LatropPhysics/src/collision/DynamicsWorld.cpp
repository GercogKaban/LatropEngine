#include "dynamics/DynamicsWorld.h"
#include <glm/vec3.hpp>

using namespace LatropPhysics;

void DynamicsWorld::applyGravity()
{
    for (CollisionBody* body : m_bodies)
    {
        RigidBody* rigidBody = dynamic_cast<RigidBody*>(body);
        if (!rigidBody) continue;
        
        rigidBody->m_force = rigidBody->m_gravity * rigidBody->m_mass;
    }
}

void DynamicsWorld::moveBodies(float deltaTime)
{
    for (CollisionBody* body : m_bodies)
    {
        RigidBody* rigidBody = dynamic_cast<RigidBody*>(body);
        if (!rigidBody) continue;
        
        glm::vec3 velocity = rigidBody->m_velocity
                           + rigidBody->m_force / rigidBody->m_mass 
                           * deltaTime;

        glm::vec3 position = rigidBody->transform->position
                           + rigidBody->m_velocity 
                           * deltaTime;

        rigidBody->m_velocity = velocity;
        rigidBody->transform->position = position;

        rigidBody->m_force = glm::vec3(0.0f);
    }
}

void DynamicsWorld::integrate(float deltaTime)
{
    applyGravity();
    resolveCollisions(deltaTime);
    moveBodies(deltaTime);
}