#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct CollisionPoints 
{
    /// @brief The furthest point of `start` into `end`.
    glm::vec3 start;

    /// @brief The furthest point of `end` into `start`.
    glm::vec3 end;

    /// @brief `end` - `start`, normalized.
    glm::vec3 normal;

    /// @brief Length of `end` - `start`.
    float depth;

    bool hasCollision;
};