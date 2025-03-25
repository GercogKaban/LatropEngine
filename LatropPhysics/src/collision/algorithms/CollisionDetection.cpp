#include "collision/algorithms/CollisionDetection.h"
#include "collision/SphereCollider.h"
#include "collision/BoundedPlaneCollider.h"
#include "collision/AABBCollider.h"
#include "shared/Transform.h"
#include "shared/AABB.h"
#include <vector>
#include <limits>

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

// MARK: - Plane
CollisionPoints collisionDetectors::findPlaneSphereCollisionPoints(
    const BoundedPlaneCollider* a, const Transform* transformA,
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

// MARK: Mixed
CollisionPoints collisionDetectors::findSpherePlaneCollisionPoints(
    const SphereCollider* a, const Transform* transformA,
    const BoundedPlaneCollider* b, const Transform* transformB
) {
    return {};
}

CollisionPoints collisionDetectors::findPlaneAABBCollisionPoints(
    const BoundedPlaneCollider* a, const Transform* transformA,
    const AABBCollider* b, const Transform* transformB
) {
    CollisionPoints points;
    points.hasCollision = false;

    // Get AABB corners in world space
    auto [aabbMin, aabbMax] = b->getAABB(transformB);

    // Transform plane normal to world space
    glm::vec3 planeNormal = glm::normalize(transformA->rotation * a->normal);

    // Transform AABB corners into plane's local space
    std::vector<glm::vec3> corners = {
        // Min side
        { aabbMin.x, aabbMin.y, aabbMin.z },
        { aabbMax.x, aabbMin.y, aabbMin.z },
        { aabbMin.x, aabbMax.y, aabbMin.z },
        { aabbMin.x, aabbMin.y, aabbMax.z },
        // Max side
        { aabbMax.x, aabbMax.y, aabbMax.z },
        { aabbMin.x, aabbMax.y, aabbMax.z },
        { aabbMax.x, aabbMin.y, aabbMax.z },
        { aabbMax.x, aabbMax.y, aabbMin.z }
    };

    // Project corners onto the plane
    int behindCount = 0;
    float minPenetration = std::numeric_limits<float>::max();
    glm::vec3 penetrationPoint;

    for (const auto& corner : corners) {
        float signedDistance = glm::dot(corner - transformA->position, planeNormal);
        if (signedDistance < 0) {
            behindCount++;
            if (-signedDistance < minPenetration) {
                minPenetration = -signedDistance;
                penetrationPoint = corner;
            }
        }
    }

    // If at least one corner is behind the plane, we have a collision
    if (behindCount > 0) {
        points.hasCollision = true;
        points.normal = planeNormal;
        points.depth = minPenetration;
        points.start = penetrationPoint;
        points.end = penetrationPoint + planeNormal * minPenetration;
    }

    return points;
}