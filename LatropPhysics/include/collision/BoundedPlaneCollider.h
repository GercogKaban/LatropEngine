#pragma once

#include "Collider.h"

namespace LP 
{
    struct BoundedPlaneCollider: Collider 
    {
        static glm::vec3 normal;

        virtual AABB getAABB(const Transform* transform) const override;

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
            const BoundedPlaneCollider* other,
            const Transform* otherTransform
        ) const override;
    };
} // namespace LP