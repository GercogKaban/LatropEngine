#pragma once

#include "collision/CollisionWorld.h"
#include "RigidBody.h"
#include <glm/vec3.hpp>

namespace LatropPhysics 
{
    struct DynamicsWorld : CollisionWorld
    {
        void addRigidBody(std::weak_ptr<RigidBody> body) 
        {
            if (body.lock()->m_takesGravity) 
            {
                body.lock()->m_gravity = m_gravity;    
            }

            if (body.lock()->m_isSimulated)
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
    // protected:
        glm::vec3 m_gravity = { 0.0f, -9.81f, 0.0f };
    };
} // namespace LatropPhysics