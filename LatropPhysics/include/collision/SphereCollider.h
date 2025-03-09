#pragma once
#include "Collider.h"

struct SphereCollider: Collider
{
    glm::vec3 center;
    float radius;

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
        const PlaneCollider* other,
        const Transform* otherTransform
    ) const override;
};