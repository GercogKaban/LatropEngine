#pragma once

#include "shared/Transform.h"
#include "CollisionPoints.h"

namespace LatropPhysics 
{

struct SphereCollider;
struct PlaneCollider;

struct Collider 
{
    virtual CollisionPoints testCollision(
        const Transform* transform,
        const Collider* other,
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