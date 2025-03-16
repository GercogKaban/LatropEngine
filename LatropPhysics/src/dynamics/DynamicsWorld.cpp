#include "dynamics/DynamicsWorld.h"
#include <glm/vec3.hpp>

using namespace LatropPhysics;

void DynamicsWorld::applyGravity()
{
    for (std::weak_ptr<CollisionBody> body : m_bodies)
    {
        if (body.expired())
        {
            // should be removed
            // for now continue
            continue;
        }
   
        if (std::shared_ptr<RigidBody> castedRigidBody = std::dynamic_pointer_cast<RigidBody>(body.lock()))
        {
            castedRigidBody->m_force = castedRigidBody->m_gravity * castedRigidBody->m_mass;
        }
    }
}

void DynamicsWorld::moveBodies(float deltaTime)
{
    for (std::weak_ptr<CollisionBody> body : m_bodies)
    {
        RigidBody* rigidBody = dynamic_cast<RigidBody*>(body.lock().get());
        if (!rigidBody) continue;
        
        glm::vec3 velocity = rigidBody->m_velocity
                           + rigidBody->m_force / rigidBody->m_mass 
                           * deltaTime;

        glm::vec3 position = rigidBody->transform.position
                           + rigidBody->m_velocity 
                           * deltaTime;

        rigidBody->m_velocity = velocity;
        rigidBody->transform.position = position;

        rigidBody->m_force = glm::vec3(0.0f);
    }
}

void DynamicsWorld::integrate(float deltaTime)
{
    applyGravity();
    resolveCollisions(deltaTime);
    moveBodies(deltaTime);
}