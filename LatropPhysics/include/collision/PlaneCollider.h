#pragma once

#include "Collider.h"

namespace LatropPhysics 
{
    struct PlaneCollider: Collider 
    {
        PlaneCollider(const glm::vec3& plane, float distance) 
            : plane(plane), 
              distance(distance)
            {}

        glm::vec3 plane;
        float distance;

        virtual CollisionPoints testCollision(
            const Transform* transform,
            const Collider* other,
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
    };
} // namespace LatropPhysics