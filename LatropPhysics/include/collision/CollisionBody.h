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
        CollisionBody(
            std::weak_ptr<Collider> collider,
            Transform transform = Transform(), 
            bool isTrigger = false, 
            std::function<void(const Collision&, float)> onCollision = [](auto a, auto b) {}
        ) : collider(collider), 
            transform(transform), 
            isTrigger(isTrigger), 
            m_onCollision(onCollision)
        {}

        virtual ~CollisionBody() = default;

    // protected:

        Transform transform;
        std::function<void(Collision, float)> m_onCollision;
        std::weak_ptr<Collider> collider;

        bool isTrigger;
    };
} // namespace LatropPhysics