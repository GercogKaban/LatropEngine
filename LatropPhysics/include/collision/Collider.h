#pragma once

#include "CollisionPoints.h"

namespace LatropPhysics 
{
    struct SphereCollider;
    struct PlaneCollider;
    struct AABBCollider;
    struct Transform;

    struct Collider 
    {
        virtual ~Collider() = default;

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
} // namespace LatropPhysics