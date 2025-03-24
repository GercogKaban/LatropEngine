#include "collision/SphereCollider.h"
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

using namespace LP;

AABB SphereCollider::getAABB(const Transform* transform) const
{ 
    auto center = this->center + transform->position;
    auto radius = this->radius * transform->scale;
    return AABB { center - radius, center + radius };
};

CollisionPoints SphereCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

// CollisionPoints SphereCollider::testCollision(
//     const Transform* transform,
//     const SphereCollider* other,
//     const Transform* otherTransform
// ) const {
//     return collisionDetectors::findSphereSphereCollisionPoints(
//         this, transform, other, otherTransform
//     );
// }

CollisionPoints SphereCollider::testCollision(
    const Transform* transform,
    const PlaneCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findSpherePlaneCollisionPoints(
        this, transform, other, otherTransform
    );
}