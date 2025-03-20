#include "dynamics/DynamicsWorld.h"

using namespace LatropPhysics;

void DynamicsWorld::applyGravity()
{
    for (const std::weak_ptr<CollisionBody>& body : movableBodies)
    {
        if (std::shared_ptr<RigidBody> rigidBody = std::dynamic_pointer_cast<RigidBody>(body.lock()))
        {
            if (rigidBody->m_isSimulated)
            {
                rigidBody->m_force = rigidBody->m_gravity * rigidBody->m_mass;
            }
            else
            {
                // TODO: Remove the expired body?
            }
        }
    }
}

void DynamicsWorld::moveBodies(float deltaTime)
{
    for (const std::weak_ptr<CollisionBody>& body : movableBodies)
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