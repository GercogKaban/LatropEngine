#include "collision/SphereCollider.h"
#include "collision/algorithms/CollisionDetection.h"

CollisionPoints SphereCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

CollisionPoints SphereCollider::testCollision(
    const Transform* transform,
    const SphereCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findSphereSphereCollisionPoints(
        this, transform, other, otherTransform
    );
}

CollisionPoints SphereCollider::testCollision(
    const Transform* transform,
    const PlaneCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findSpherePlaneCollisionPoints(
        this, transform, other, otherTransform
    );
}