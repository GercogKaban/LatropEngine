#include "collision/AABBCollider.h"
#include "collision/algorithms/CollisionDetection.h"

using namespace LatropPhysics;

CollisionPoints AABBCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

// CollisionPoints AABBCollider::testCollision(
//     const Transform* transform,
//     const SphereCollider* other,
//     const Transform* otherTransform
// ) const {
//     
// }

CollisionPoints AABBCollider::testCollision(
    const Transform* transform,
    const PlaneCollider* other,
    const Transform* otherTransform
) const {
    // We don't really care about plane-plane collisions.
    return {};
};