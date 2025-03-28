#pragma once

#include <glm/vec3.hpp>

namespace LP 
{
    struct CollisionBody;

    struct CollisionPoints 
    {
        /// @brief The furthest point of `start` into `end`.
        glm::vec3 start = glm::vec3(0.0f);

        /// @brief The furthest point of `end` into `start`.
        glm::vec3 end = glm::vec3(0.0f);

        /// @brief `end` - `start`, normalized.
        glm::vec3 normal = glm::vec3(0.0f);

        /// @brief Length of `end` - `start`.
        float depth = 0.0f;

        bool hasCollision = false;
    };

    struct Collision
    {
        Collision(CollisionBody* bodyA, CollisionBody* bodyB, const CollisionPoints& points)
            : bodyA(bodyA), 
              bodyB(bodyB), 
              points(points)
            {}

        CollisionPoints points;
        CollisionBody* bodyA;
        CollisionBody* bodyB;
    };
} // namespace LP