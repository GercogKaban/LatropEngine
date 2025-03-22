#include "dynamics/DynamicsWorld.h"

using namespace LP;

void DynamicsWorld::applyGravity()
{
    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        if (std::shared_ptr<RigidBody> rigidBody = body.lock())
        {
            if (rigidBody->m_isSimulated && rigidBody->m_takesGravity)
            {
                rigidBody->m_force = m_gravity * rigidBody->m_mass;
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
    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        if (std::shared_ptr<RigidBody> rigidBody = body.lock())
        {
            glm::vec3 oldVelocity = rigidBody->m_velocity;
            glm::vec3 oldPosition = rigidBody->transform.position;

            rigidBody->m_velocity = oldVelocity + rigidBody->m_force / rigidBody->m_mass * deltaTime;
            rigidBody->transform.position = oldPosition + oldVelocity * deltaTime;

            rigidBody->m_force = glm::vec3(0.0f);
        }
    }
}

void DynamicsWorld::integrate(float deltaTime)
{
    applyGravity();
    resolveCollisions(deltaTime);
    moveBodies(deltaTime);
}