#include "collision/PlaneCollider.h"
#include "collision/algorithms/CollisionDetection.h"

CollisionPoints PlaneCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

CollisionPoints PlaneCollider::testCollision(
    const Transform* transform,
    const SphereCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findPlaneSphereCollisionPoints(
        this, transform, other, otherTransform
    );
}

CollisionPoints PlaneCollider::testCollision(
    const Transform* transform,
    const PlaneCollider* other,
    const Transform* otherTransform
) const {
    // We don't really care about plane-plane collisions.
    return {};
};