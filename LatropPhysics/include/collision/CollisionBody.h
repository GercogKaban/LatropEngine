#pragma once

#include "Collider.h"
#include "shared/Transform.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>
#include <memory>

namespace LatropPhysics 
{
    struct CollisionBody
    {
        CollisionBody(){}

        virtual ~CollisionBody() = default;

    // protected:

        Transform transform {};
        std::function<void(Collision, float)> m_onCollision = [](auto a, auto b) {};
        std::weak_ptr<Collider> collider;

        bool isTrigger = false;
    };
} // namespace LatropPhysics