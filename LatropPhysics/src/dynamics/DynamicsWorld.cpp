#include "dynamics/DynamicsWorld.h"

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
        auto element = std::dynamic_pointer_cast<RigidBody>(body.lock());
        if (!element) continue;
        
        auto rigidBody = element.get();

        glm::vec3 oldVelocity = rigidBody->m_velocity;
        glm::vec3 oldPosition = rigidBody->transform.position;

        rigidBody->m_velocity = oldVelocity + rigidBody->m_force / rigidBody->m_mass * deltaTime;
        rigidBody->transform.position = oldPosition + oldVelocity * deltaTime;

        rigidBody->m_force = glm::vec3(0.0f);
    }
}

void DynamicsWorld::integrate(float deltaTime)
{
    applyGravity();
    resolveCollisions(deltaTime);
    moveBodies(deltaTime);
}