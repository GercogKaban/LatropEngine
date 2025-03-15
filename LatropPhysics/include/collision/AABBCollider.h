#pragma once

#include "Collider.h"
#include <glm/vec3.hpp>

namespace LatropPhysics 
{

/// @brief The AABB class represents an Axis Aligned Bounding Box that can be 
/// used as a collider in a physics engine.
struct AABBCollider: Collider
{
public:
    /** 
	 * Creates an AABB in a usable state.
	 * 
	 * @param minExtents The corner of the AABB with the smallest coordinates.
	 * @param maxExtents The corner of the AABB with the largest coordinates.
	 */
	AABBCollider(const glm::vec3& minExtents, const glm::vec3& maxExtents) :
		minExtents(minExtents),
		maxExtents(maxExtents) {}

    virtual CollisionPoints testCollision(
        const Transform* transform,
        const Collider* other,
        const Transform* otherTransform
    ) const override;
    
    // virtual CollisionPoints testCollision(
    //     const Transform* transform,
    //     const SphereCollider* other,
    //     const Transform* otherTransform
    // ) const override;

    virtual CollisionPoints testCollision(
        const Transform* transform,
        const PlaneCollider* other,
        const Transform* otherTransform
    ) const override;

    /// @brief The corner of the AABB with the smallest coordinates.
    const glm::vec3 minExtents;

    /// @brief The corner of the AABB with the largest coordinates.
    const glm::vec3 maxExtents;
};

} // namespace LatropPhysics