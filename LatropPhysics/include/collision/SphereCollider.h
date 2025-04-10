#pragma once

#include "Collider.h"

namespace LP 
{
    struct SphereCollider: Collider
    {
        SphereCollider(float radius) : radius(radius) {}

        float radius;

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
    };
} // namespace LP