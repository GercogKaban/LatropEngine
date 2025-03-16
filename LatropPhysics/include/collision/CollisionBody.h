#pragma once

#include "Collider.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>

namespace LatropPhysics 
{

struct CollisionBody
{
    virtual ~CollisionBody() = default;

// protected:

    Collider* collider = nullptr;
    Transform transform;

    bool isTrigger;
    
    std::function<void(Collision, float)> m_onCollision;
};

} // namespace LatropPhysics