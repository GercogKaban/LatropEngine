#pragma once

#include "Collider.h"

namespace LP 
{
    struct CapsuleCollider: Collider 
    {
        CapsuleCollider(float radius) : radius(radius) {}

        const float radius;

        void getWorldEndpoints(const Transform* transform, glm::vec3& top, glm::vec3& bottom) const;
        
        virtual AABB getAABB(const Transform* transform) const override;

        // MARK: - Colliders

        virtual ContactManifold testCollision(
            const Transform* transform,
            const Collider* other,
            const Transform* otherTransform
        ) const override;

        virtual ContactManifold testCollision(
            const Transform* transform,
            const SphereCollider* other,
            const Transform* otherTransform
        ) const override;

        virtual ContactManifold testCollision(
            const Transform* transform,
            const CapsuleCollider* other,
            const Transform* otherTransform
        ) const override;

        virtual ContactManifold testCollision(
            const Transform* transform,
            const BoundedPlaneCollider* other,
            const Transform* otherTransform
        ) const override;

        virtual ContactManifold testCollision(
            const Transform* transform,
            const OBBCollider* other,
            const Transform* otherTransform
        ) const override;
    };
} // namespace LP