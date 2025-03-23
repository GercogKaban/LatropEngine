#pragma once

#include "Collider.h"
#include <glm/vec3.hpp>
#include "shared/Transform.h"

namespace LP 
{
    /// @brief The AABB class represents an Axis Aligned Bounding Box that can be 
    /// used as a collider in a physics engine.
    struct AABBCollider : Collider
    {
        /**
         * Creates an AABB in a usable state.
         *
         * @param minExtents The corner of the AABB with the smallest coordinates.
         * @param maxExtents The corner of the AABB with the largest coordinates.
         */
        AABBCollider(const glm::vec3& minExtents, const glm::vec3& maxExtents) 
            : minExtents(minExtents),
              maxExtents(maxExtents) 
            {}

        virtual AABB getAABB(Transform * transform) const override
        { 
            glm::vec3 aMin = /*transform->position + */minExtents * transform->scale;
            glm::vec3 aMax = /*transform->position + */maxExtents * transform->scale;

            return { aMin, aMax };
        };

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const Collider* other,
            const Transform* otherTransform
        ) const override;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const AABBCollider* other,
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

        // MARK: - Common Shapes
        static AABBCollider makeCube() 
        {
            return AABBCollider({ -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f });
        }
    };
} // namespace LP