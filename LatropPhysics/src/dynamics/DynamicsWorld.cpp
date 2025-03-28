#include "dynamics/DynamicsWorld.h"

using namespace LP;

void DynamicsWorld::applyGravity()
{
    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        if (std::shared_ptr<RigidBody> rigidBody = body.lock())
        {
            if (rigidBody->isSimulated() && rigidBody->takesGravity)
            {
                rigidBody->force = m_gravity * rigidBody->getMass();
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
            glm::vec3 oldVelocity = rigidBody->linearVelocity;
            glm::vec3 oldPosition = rigidBody->transform.position;

            rigidBody->linearVelocity = oldVelocity + rigidBody->force * rigidBody->getInvMass() * deltaTime;
            rigidBody->transform.position = oldPosition + oldVelocity * deltaTime;

            rigidBody->force = glm::vec3(0.0f);
        }
    }
}

void DynamicsWorld::integrate(float deltaTime)
{
    applyGravity();
    resolveCollisions(deltaTime);
    moveBodies(deltaTime);
}