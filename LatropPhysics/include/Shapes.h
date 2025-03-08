#pragma once
#include <glm/vec3.hpp>

/// @brief An infinite flat surface that can be used to clip, reflect, 
/// or constrain other objects.
struct Plane
{
    /// @brief The normal vector (a unit vector perpendicula to the plane).
    glm::vec3 normal;

    /// @brief Distance from the origin along the normal(or negative dot product
    /// with a point on the plane).
    float distance;
};

struct Capsule
{
    /// @brief The starting endpoint of the capsule's linepoint.
    glm::vec3 start;

    /// @brief The ending endpoint of the capsule's linepoint.
    glm::vec3 end;

    /// @brief The radius of the capsule.
    float radius;
};