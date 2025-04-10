#pragma once

#include "Collider.h"
#include <glm/vec3.hpp>
#include "shared/Transform.h"

namespace LP
{
    /// @brief The OBB struct represents an Oriented Bounding Box that can be 
    /// used as a collider in a physics engine. Unlike an AABB, OBB can be rotated.
    struct OBBCollider : Collider
    {
        /**
         * Creates an OBB in a usable state.
         *
         * @param minExtents The corner of the OBB with the smallest coordinates, not rotated.
         * @param maxExtents The corner of the OBB with the largest coordinates, not rotated.
         */
        OBBCollider(const glm::vec3& minExtents, const glm::vec3& maxExtents) 
            : minExtents(minExtents),
              maxExtents(maxExtents) 
            {}

        /// @brief The corner of the OBB with the smallest coordinates, not rotated.
        const glm::vec3 minExtents;

        /// @brief The corner of the OBB with the largest coordinates, not rotated.
        const glm::vec3 maxExtents;

        virtual AABB getAABB(const Transform* transform) const override;

        // MARK: - Colliders

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const Collider* other,
            const Transform* otherTransform
        ) const override;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const SphereCollider* other,
            const Transform* otherTransform
        ) const override;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const CapsuleCollider* other,
            const Transform* otherTransform
        ) const override;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const BoundedPlaneCollider* other,
            const Transform* otherTransform
        ) const override;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const OBBCollider* other,
            const Transform* otherTransform
        ) const override;

        // MARK: - Common Shapes
        
        static OBBCollider makeCube() 
        {
            return OBBCollider({ -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f });
        }
    };
} // namespace LP