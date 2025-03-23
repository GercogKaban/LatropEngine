#pragma once

#include "CollisionPoints.h"

namespace LP 
{
    struct SphereCollider;
    struct PlaneCollider;
    struct AABBCollider;
    struct Transform;

    struct AABB
    {
        glm::vec3 minExtents;
        glm::vec3 maxExtents;
    };

    struct Collider 
    {
        virtual ~Collider() = default;

        virtual AABB getAABB(Transform * transform) const
        { 
            return AABB { glm::vec3(0), glm::vec3(0)};
        };

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
            const PlaneCollider* other,
            const Transform* otherTransform
        ) const = 0;
    };
} // namespace LP