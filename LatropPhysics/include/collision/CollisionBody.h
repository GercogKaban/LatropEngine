#pragma once

#include "Collider.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>
#include <memory>

namespace LatropPhysics 
{

struct CollisionBody
{
    virtual ~CollisionBody() = default;

// protected:

    std::weak_ptr<Collider> collider;
    Transform transform;

    bool isTrigger = false;
    
    std::function<void(Collision, float)> m_onCollision;
};

} // namespace LatropPhysics