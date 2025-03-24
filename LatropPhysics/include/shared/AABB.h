#pragma once

#include <glm/vec3.hpp>

namespace LP
{
    /// @brief Axis-aligned bounding box is, as the name tells, cannot rotated
    /// and is always aligned along the corresponding axes.
    struct AABB
    {
        /// @brief The corner of the AABB with the smallest coordinates.
        glm::vec3 minExtents;

        /// @brief The corner of the AABB with the largest coordinates.
        glm::vec3 maxExtents;
    };
} // namespace LP