#pragma once

#include "Collider.h"

namespace LP 
{
    struct SphereCollider: Collider
    {
        SphereCollider(const glm::vec3& center, float radius)
            : center(center),
              radius(radius)
            {}

        glm::vec3 center;
        float radius;

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
} // namespace LP