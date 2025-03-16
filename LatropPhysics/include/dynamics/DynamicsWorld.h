#pragma once

#include "collision/CollisionWorld.h"
#include "RigidBody.h"
#include <glm/vec3.hpp>

namespace LatropPhysics 
{

struct DynamicsWorld: CollisionWorld
{
public:

    void addRigidBody(std::weak_ptr<RigidBody> body) 
    {
        if (body.lock().get()->m_takesGravity) 
        {
            body.lock().get()->m_gravity = m_gravity;
        }

        addCollisionBody(body);
    }

    void integrate(float deltaTime);
// protected:
    glm::vec3 m_gravity = { 0.0f, -9.81f, 0.0f };

private:
    void applyGravity();
    void moveBodies(float deltaTime);
};

} // namespace LatropPhysics