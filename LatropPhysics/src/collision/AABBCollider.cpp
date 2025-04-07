#include "collision/AABBCollider.h"
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

using namespace LP;

AABB AABBCollider::getAABB(const Transform* transform) const
{ 
    glm::vec3 aMin = transform->position + minExtents * transform->scale;
    glm::vec3 aMax = transform->position + maxExtents * transform->scale;

    return { aMin, aMax };
};

// MARK: Colliders

CollisionPoints AABBCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

CollisionPoints AABBCollider::testCollision(
    const Transform* transform,
    const SphereCollider* other,
    const Transform* otherTransform
) const {
    return {};   
}

CollisionPoints AABBCollider::testCollision(
    const Transform* transform,
    const CapsuleCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findAABBCapsuleCollisionPoints(
        this, transform, other, otherTransform
    );
}

CollisionPoints AABBCollider::testCollision(
    const Transform* transform,
    const BoundedPlaneCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findPlaneAABBCollisionPoints(other, otherTransform, this, transform);
};

CollisionPoints AABBCollider::testCollision(
    const Transform* transform,
    const AABBCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findAABBAABBCollisionPoints(
        this, transform, other, otherTransform
    );
}

CollisionPoints AABBCollider::testCollision(
    const Transform* transform,
    const OBBCollider* other,
    const Transform* otherTransform
) const {
    return {};   
}