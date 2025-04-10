#include "collision/SphereCollider.h"
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

using namespace LP;

AABB SphereCollider::getAABB(const Transform* transform) const
{ 
    auto center = transform->position;
    auto radius = this->radius * transform->scale;
    return AABB { center - radius, center + radius };
};

ContactManifold SphereCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

ContactManifold SphereCollider::testCollision(
    const Transform* transform,
    const SphereCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findSphereSphereCollisionPoints(
        this, transform, other, otherTransform
    );
}

ContactManifold SphereCollider::testCollision(
    const Transform* transform,
    const CapsuleCollider* other,
    const Transform* otherTransform
) const {
    return {};
}

ContactManifold SphereCollider::testCollision(
    const Transform* transform,
    const BoundedPlaneCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findPlaneSphereCollisionPoints(
        other, otherTransform, this, transform
    );
}

ContactManifold SphereCollider::testCollision(
    const Transform* transform,
    const OBBCollider* other,
    const Transform* otherTransform
) const {
    return {};
}