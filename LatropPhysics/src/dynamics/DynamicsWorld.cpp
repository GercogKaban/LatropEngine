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
                rigidBody->force = gravity * rigidBody->getMass();
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

            // Update linear velocity
            rigidBody->linearVelocity = oldVelocity + rigidBody->force * rigidBody->getInvMass() * deltaTime;
            rigidBody->transform.position = oldPosition + oldVelocity * deltaTime;

            // Update angular velocity based on torque and inertia
            glm::vec3 angularAcceleration = rigidBody->torque * rigidBody->invInertiaTensorLocal;
            rigidBody->angularVelocity += angularAcceleration * deltaTime;

            // Update rotation based on angular velocity
            glm::quat rotationDelta = glm::quat(0, rigidBody->angularVelocity.x, rigidBody->angularVelocity.y, rigidBody->angularVelocity.z) * deltaTime;
            glm::vec3 axis = glm::normalize(rigidBody->angularVelocity);
            float angle = glm::length(rigidBody->angularVelocity) * deltaTime;

            if (angle > 0.0f) {
                glm::quat deltaRotation = glm::angleAxis(angle, axis);
                rigidBody->transform.rotation = glm::normalize(deltaRotation * rigidBody->transform.rotation);
            }

            rigidBody->force = glm::vec3(0.0f);
            rigidBody->torque = glm::vec3(0.0f);  // Reset torque after applying it
        }
    }
}

void DynamicsWorld::integrate(float deltaTime)
{
    applyGravity();
    resolveCollisions(deltaTime * timeScale);
    moveBodies(deltaTime * timeScale);
}