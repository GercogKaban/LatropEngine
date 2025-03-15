#pragma once

#include "Collider.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>

namespace LatropPhysics 
{

struct CollisionBody
{
public:
    virtual ~CollisionBody() = default;

// protected:
    Collider* collider;
    Transform* transform;

    // bool isDynamic;
    bool isTrigger;
    
    std::function<void(Collision, float)> m_onCollision;
};

} // namespace LatropPhysics