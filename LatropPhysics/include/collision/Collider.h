#pragma once

#include "CollisionPoints.h"

namespace LP 
{
    struct SphereCollider;
    struct BoundedPlaneCollider;
    struct AABBCollider;
    struct Transform;
    struct AABB;

    struct Collider 
    {
        virtual ~Collider() = default;

        virtual AABB getAABB(const Transform * transform) const = 0;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const Collider* other,
            const Transform* otherTransform
        ) const = 0;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const AABBCollider* other,
            const Transform* otherTransform
        ) const = 0;
        
        // virtual CollisionPoints testCollision(
        //     const Transform* transform,
        //     const SphereCollider* other,
        //     const Transform* otherTransform
        // ) const = 0;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const BoundedPlaneCollider* other,
            const Transform* otherTransform
        ) const = 0;
    };
} // namespace LP