#include "collision/algorithms/CollisionDetection.h"
#include "collision/SphereCollider.h"
#include "collision/PlaneCollider.h"
#include "collision/AABBCollider.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

using namespace LP;

// MARK: - Sphere
CollisionPoints collisionDetectors::findSphereSphereCollisionPoints(
    const SphereCollider* a, const Transform* transformA,
    const SphereCollider* b, const Transform* transformB
) {
    glm::vec3 centerA = transformA->position + a->center;
    glm::vec3 centerB = transformB->position + b->center;
    glm::vec3 delta = centerB - centerA;
    float distance = glm::length(delta);
    float combinedRadius = a->radius + b->radius;

    CollisionPoints result;
    result.hasCollision = distance < combinedRadius;
    result.normal = distance > 0.0f ? glm::normalize(delta) : glm::vec3(1, 0, 0);
    result.start = centerA + result.normal * a->radius;
    result.end = centerB - result.normal * b->radius;
    result.depth = combinedRadius - distance;

    return result;
}

CollisionPoints collisionDetectors::findSpherePlaneCollisionPoints(
    const SphereCollider* a, const Transform* transformA,
    const PlaneCollider* b, const Transform* transformB
) {
    glm::vec3 center = transformA->position + a->center;
    glm::vec3 planeNormal = glm::normalize(b->plane);
    float signedDistance = glm::dot(center, planeNormal) - b->distance;

    CollisionPoints result;
    result.hasCollision = signedDistance < a->radius;
    result.normal = planeNormal;
    result.start = center - planeNormal * a->radius;
    result.end = center - planeNormal * signedDistance;
    result.depth = a->radius - signedDistance;

    return result;
}

// MARK: - Plane
CollisionPoints collisionDetectors::findPlaneSphereCollisionPoints(
    const PlaneCollider* a, const Transform* transformA,
    const SphereCollider* b, const Transform* transformB
) {
    return findSpherePlaneCollisionPoints(b, transformB, a, transformA);
}

// MARK: - AABB
CollisionPoints collisionDetectors::findAABBAABBCollisionPoints(
    const AABBCollider* a, const Transform* transformA,
    const AABBCollider* b, const Transform* transformB
) {
    CollisionPoints points;
    points.hasCollision = false;

    auto [aMin, aMax] = a->getAABB(transformA);
    auto [bMin, bMax] = b->getAABB(transformB);

    // Calculate overlap on each axis
    glm::vec3 overlapMin = glm::max(aMin, bMin);
    glm::vec3 overlapMax = glm::min(aMax, bMax);

    // Check for collision
    if (overlapMin.x >= overlapMax.x || overlapMin.y >= overlapMax.y || overlapMin.z >= overlapMax.z) {
        return points;  // No collision
    }

    // Collision exists
    points.hasCollision = true;

    // Calculate overlap distances along each axis
    float overlapX = overlapMax.x - overlapMin.x;
    float overlapY = overlapMax.y - overlapMin.y;
    float overlapZ = overlapMax.z - overlapMin.z;

    // Find the smallest overlap axis (to calculate the collision normal and depth)
    float minOverlap = overlapX;
    points.normal = glm::vec3((aMin.x < bMin.x) ? -1.0f : 1.0f, 0.0f, 0.0f);

    if (overlapY < minOverlap) {
        minOverlap = overlapY;
        points.normal = glm::vec3(0.0f, (aMin.y < bMin.y) ? -1.0f : 1.0f, 0.0f);
    }

    if (overlapZ < minOverlap) {
        minOverlap = overlapZ;
        points.normal = glm::vec3(0.0f, 0.0f, (aMin.z < bMin.z) ? -1.0f : 1.0f);
    }

    // Calculate penetration depth
    points.depth = minOverlap;

    // Calculate collision points
    points.start = overlapMin;
    points.end = overlapMax;

    return points;
}