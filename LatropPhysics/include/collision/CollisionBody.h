#pragma once

#include "Collider.h"
#include "shared/Transform.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>
#include <memory>

namespace LP 
{
    struct CollisionBody
    {
        virtual ~CollisionBody() = default;

        Transform transform {};
        std::function<void(Collision, float)> onCollision = [](auto a, auto b) {};
        std::weak_ptr<Collider> collider;

        /// @brief If the body is a trigger it is not affected by collisions
        /// nor does it affect other bodies. Only onCollision callback is called.
        bool isTrigger = false;

        /// @brief If the body is simulated and moves, or only affects others.
        inline bool isSimulated() const { return m_isSimulated; }
        virtual void setIsSimulated(bool newValue) { m_isSimulated = newValue; }

    private:
        /// @brief If the body is simulated and moves, or only affects others.
        bool m_isSimulated = true;
    };
} // namespace LP