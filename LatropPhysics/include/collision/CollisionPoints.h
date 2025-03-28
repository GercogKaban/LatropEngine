#pragma once

#include <glm/vec3.hpp>

namespace LP 
{
    struct CollisionBody;

    struct CollisionPoints 
    {
        // CollisionPoints(glm::vec3 start, glm::vec3 end, glm::vec3 normal, float depth, bool hasCollision)
        //     : start(start), end(end), normal(normal), depth(depth), hasCollision(hasCollision)
        //     {}

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

    struct CollisionManifold
    {
        glm::vec3 contactPoints[2];
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