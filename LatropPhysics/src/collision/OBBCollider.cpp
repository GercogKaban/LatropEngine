#include "collision/OBBCollider.h"
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"
#include "shared/AABB.h"
#include <float.h>

using namespace LP;

AABB OBBCollider::getAABB(const Transform* transform) const
{
    glm::mat3 rotation = glm::mat3_cast(transform->rotation);
    glm::vec3 center = transform->position;
    glm::vec3 scaledMin = minExtents * transform->scale;
    glm::vec3 scaledMax = maxExtents * transform->scale;

    glm::vec3 corners[8] = {
        scaledMin,
        {scaledMax.x, scaledMin.y, scaledMin.z},
        {scaledMin.x, scaledMax.y, scaledMin.z},
        {scaledMin.x, scaledMin.y, scaledMax.z},
        {scaledMax.x, scaledMax.y, scaledMin.z},
        {scaledMax.x, scaledMin.y, scaledMax.z},
        {scaledMin.x, scaledMax.y, scaledMax.z},
        scaledMax
    };

    glm::vec3 minAABB = glm::vec3(FLT_MAX);
    glm::vec3 maxAABB = glm::vec3(-FLT_MAX);

    for (int i = 0; i < 8; ++i) {
        glm::vec3 worldCorner = center + rotation * corners[i];
        minAABB = glm::min(minAABB, worldCorner);
        maxAABB = glm::max(maxAABB, worldCorner);
    }

    return { minAABB, maxAABB };
}

// MARK: Colliders

ContactManifold OBBCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

ContactManifold OBBCollider::testCollision(
    const Transform* transform,
    const SphereCollider* other,
    const Transform* otherTransform
) const {
    return {};   
}

ContactManifold OBBCollider::testCollision(
    const Transform* transform,
    const CapsuleCollider* other,
    const Transform* otherTransform
) const {
    return {};
}

ContactManifold OBBCollider::testCollision(
    const Transform* transform,
    const BoundedPlaneCollider* other,
    const Transform* otherTransform
) const {
    return {};
};

ContactManifold OBBCollider::testCollision(
    const Transform* transform,
    const OBBCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findOBBOBBCollisionPoints(this, transform, other, otherTransform);   
}