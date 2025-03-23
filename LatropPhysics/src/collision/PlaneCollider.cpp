#include "collision/PlaneCollider.h"
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

using namespace LP;

AABB PlaneCollider::getAABB(const Transform * transform) const
{ 
    return AABB { glm::vec3(0), glm::vec3(0)};
};

CollisionPoints PlaneCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

// CollisionPoints PlaneCollider::testCollision(
//     const Transform* transform,
//     const SphereCollider* other,
//     const Transform* otherTransform
// ) const {
//     return collisionDetectors::findPlaneSphereCollisionPoints(
//         this, transform, other, otherTransform
//     );
// }

CollisionPoints PlaneCollider::testCollision(
    const Transform* transform,
    const PlaneCollider* other,
    const Transform* otherTransform
) const {
    // We don't really care about plane-plane collisions.
    return {};
};