#include "collision/CapsuleCollider.h"
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

using namespace LP;

void CapsuleCollider::getWorldEndpoints(const Transform* transform, glm::vec3& top, glm::vec3& bottom) const
{
    float halfHeight = (1.0 - radius) * transform->scale.y; // Scale only affects height

    glm::vec3 up = transform->rotation * glm::vec3(0.0f, halfHeight, 0.0f);
    glm::vec3 down = transform->rotation * glm::vec3(0.0f, -halfHeight, 0.0f);

    top = transform->position + up;
    bottom = transform->position + down;
}

AABB CapsuleCollider::getAABB(const Transform* transform) const 
{
    glm::vec3 top, bottom;
    
    // Get the rotated world endpoints (top and bottom) of the capsule
    getWorldEndpoints(transform, top, bottom);

    // Calculate the AABB using the top and bottom points
    glm::vec3 min = glm::min(top, bottom);
    glm::vec3 max = glm::max(top, bottom);
    
    // Include the radius in the X and Z directions, as the capsule is a cylinder
    min.x -= radius * transform->scale.x;
    max.x += radius * transform->scale.x;
    
    min.z -= radius * transform->scale.z;
    max.z += radius * transform->scale.z;

    // Return the AABB for the capsule
    return AABB { min, max };
}

ContactManifold CapsuleCollider::testCollision(
    const Transform* transform,
    const Collider* other,
    const Transform* otherTransform
) const {
    return other->testCollision(otherTransform, this, transform);
}

ContactManifold CapsuleCollider::testCollision(
    const Transform* transform,
    const SphereCollider* other,
    const Transform* otherTransform
) const {
    return {};
}

ContactManifold CapsuleCollider::testCollision(
    const Transform* transform,
    const CapsuleCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findCapsuleCapsuleCollisionPoints(
        this, transform, other, otherTransform
    );
}

ContactManifold CapsuleCollider::testCollision(
    const Transform* transform,
    const BoundedPlaneCollider* other,
    const Transform* otherTransform
) const {
    return collisionDetectors::findPlaneCapsuleCollisionPoints(
        other, otherTransform, this, transform
    );
}

ContactManifold CapsuleCollider::testCollision(
    const Transform* transform,
    const OBBCollider* other,
    const Transform* otherTransform
) const {
    return {};
}