#include "collision/BoundedPlaneCollider.h"
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

using namespace LP;

AABB BoundedPlaneCollider::getAABB(const Transform* transform) const
{ 
    glm::vec3 aMin = transform->position - transform->scale / 2.0f;
    glm::vec3 aMax = transform->position + transform->scale / 2.0f;

    return { aMin, aMax };
};

CollisionPoints BoundedPlaneCollider::testCollision(
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

CollisionPoints BoundedPlaneCollider::testCollision(
    const Transform* transform,
    const BoundedPlaneCollider* other,
    const Transform* otherTransform
) const {
    // We don't really care about plane-plane collisions.
    return {};
};

CollisionPoints BoundedPlaneCollider::testCollision(
    const Transform* transform,
    const AABBCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findPlaneAABBCollisionPoints(this, transform, other, otherTransform);
};