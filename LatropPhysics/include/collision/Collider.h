#pragma once

#include "CollisionPoints.h"

namespace LP 
{
    struct SphereCollider;
    struct CapsuleCollider;
    struct BoundedPlaneCollider;
    struct OBBCollider;
    struct Transform;
    struct AABB;

    struct Collider 
    {
        virtual ~Collider() = default;

        // MARK: Broad-Phase Primitives

        virtual AABB getAABB(const Transform * transform) const = 0;

        // MARK: Narrow-Phase Primitives

        virtual ContactManifold testCollision(
            const Transform* transform,
            const Collider* other,
            const Transform* otherTransform
        ) const = 0;

        virtual ContactManifold testCollision(
            const Transform* transform,
            const SphereCollider* other,
            const Transform* otherTransform
        ) const = 0;

        virtual ContactManifold testCollision(
            const Transform* transform,
            const CapsuleCollider* other,
            const Transform* otherTransform
        ) const = 0;

        virtual ContactManifold testCollision(
            const Transform* transform,
            const BoundedPlaneCollider* other,
            const Transform* otherTransform
        ) const = 0;

        virtual ContactManifold testCollision(
            const Transform* transform,
            const OBBCollider* other,
            const Transform* otherTransform
        ) const = 0;
    };
} // namespace LP