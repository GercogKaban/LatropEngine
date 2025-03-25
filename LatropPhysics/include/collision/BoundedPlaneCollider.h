#pragma once

#include "Collider.h"

namespace LP 
{
    struct BoundedPlaneCollider: Collider 
    {
        BoundedPlaneCollider(const glm::vec3& normal) : normal(normal) {}

        glm::vec3 normal;

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

        // MARK: - Common Orientations
        static BoundedPlaneCollider makeXForward() 
        {
            return BoundedPlaneCollider({ 1.0f, 0.0f, 0.0f });
        }

        static BoundedPlaneCollider makeXBackward() 
        {
            return BoundedPlaneCollider({ -1.0f, 0.0f, 0.0f });
        }

        static BoundedPlaneCollider makeYForward() 
        {
            return BoundedPlaneCollider({ 0.0f, 1.0f, 0.0f });
        }

        static BoundedPlaneCollider makeYBackward() 
        {
            return BoundedPlaneCollider({ 0.0f, -1.0f, 0.0f });
        }

        static BoundedPlaneCollider makeZForward() 
        {
            return BoundedPlaneCollider({ 0.0f, 0.0f, -1.0f });
        }

        static BoundedPlaneCollider makeZBackward() 
        {
            return BoundedPlaneCollider({ 0.0f, 0.0f, 1.0f });
        }
    };
} // namespace LP