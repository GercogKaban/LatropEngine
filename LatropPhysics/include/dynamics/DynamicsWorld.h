#pragma once

#include "collision/CollisionWorld.h"
#include "RigidBody.h"
#include <glm/vec3.hpp>

namespace LP 
{
    struct DynamicsWorld : CollisionWorld
    {
        glm::vec3 gravity = { 0.0f, -9.81f, 0.0f };
        float timeScale = 1.0f;

        void addRigidBody(std::weak_ptr<RigidBody> body) 
        {
            if (body.lock()->isSimulated())
            {
                movableBodies.push_back(body);
            }
            else
            {
                addCollisionBody(body);
            }
        }

        virtual void integrate(float deltaTime);
        virtual void applyGravity();
        virtual void moveBodies(float deltaTime);
    };
} // namespace LP