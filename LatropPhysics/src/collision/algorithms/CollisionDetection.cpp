#include "collision/algorithms/CollisionDetection.h"
#include "collision/SphereCollider.h"
#include "collision/CapsuleCollider.h"
#include "collision/BoundedPlaneCollider.h"
#include "collision/OBBCollider.h"
#include "shared/Transform.h"
#include "shared/AABB.h"
#include <vector>
#include <limits>

using namespace LP;

// MARK: Broad Phase
// MARK: - AABB

bool collisionDetectors::testAABBAABBForCollision(const AABB& a, const AABB& b)
{
    auto [aMin, aMax] = a;
    auto [bMin, bMax] = b;

    // Calculate overlap on each axis
    glm::vec3 overlapMin = glm::max(aMin, bMin);
    glm::vec3 overlapMax = glm::min(aMax, bMax);

    // Check for collision
    if (overlapMin.x >= overlapMax.x || overlapMin.y >= overlapMax.y || overlapMin.z >= overlapMax.z) {
        return false;  // No collision
    }

    // Collision exists
    return true;
}

// MARK: Narrow Phase
// MARK: - Sphere

// CollisionPoints collisionDetectors::findSphereSphereCollisionPoints(
//     const SphereCollider* a, const Transform* transformA,
//     const SphereCollider* b, const Transform* transformB
// ) {
//     glm::vec3 centerA = transformA->position;
//     glm::vec3 centerB = transformB->position;
//     glm::vec3 delta = centerB - centerA;
//     float distance = glm::length(delta);
//     float combinedRadius = a->radius + b->radius;

//     CollisionPoints result;
//     result.hasCollision = distance < combinedRadius;
//     result.normal = distance > 0.0f ? glm::normalize(delta) : glm::vec3(1, 0, 0);
//     result.start = centerA + result.normal * a->radius;
//     result.end = centerB - result.normal * b->radius;
//     result.depth = combinedRadius - distance;

//     return result;
// }

// MARK: - Capsule

// CollisionPoints collisionDetectors::findCapsuleCapsuleCollisionPoints(
//     const CapsuleCollider* a, const Transform* transformA,
//     const CapsuleCollider* b, const Transform* transformB
// ) {
//    // Get world-space endpoints of both capsules
//    glm::vec3 topA, bottomA, topB, bottomB;
//    a->getWorldEndpoints(transformA, topA, bottomA);
//    b->getWorldEndpoints(transformB, topB, bottomB);

//    // Calculate the vector between the capsule centers
//    glm::vec3 delta = bottomB - bottomA;
//    float distance = glm::length(delta);
//    float combinedRadius = a->radius + b->radius;

//    CollisionPoints result;
//    result.hasCollision = false;

//    // Check if the distance between the capsule centers is less than the combined radii
//    if (distance < combinedRadius) {
//        // There is a potential collision, check further
//        result.hasCollision = true;

//        // Calculate normal vector between capsules
//        result.normal = glm::normalize(delta);

//        // Calculate the penetration depth (how far the capsules are overlapping)
//        result.depth = combinedRadius - distance;

//        // Calculate collision points (adjusted by radius to find exact collision)
//        result.start = bottomA + result.normal * a->radius;
//        result.end = bottomB - result.normal * b->radius;
//    }

//    return result;
// }

// MARK: OBB

std::array<glm::vec3, 8> getOBBCorners(const OBBCollider* obb, const Transform* transform) {
    glm::vec3 extents = (obb->maxExtents - obb->minExtents) * 0.5f;

    glm::mat3 rot = glm::mat3_cast(transform->rotation);

    assert(glm::length(transform->scale) > 0.0f);

    std::array<glm::vec3, 8> corners;
    int i = 0;
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                glm::vec3 offset = glm::vec3(x * extents.x, y * extents.y, z * extents.z);
                glm::vec3 scaledOffset = offset * transform->scale;
                corners[i++] = transform->position + rot * scaledOffset;
            }
        }
    }

    return corners;
}

std::pair<float, float> projectOntoAxis(const std::array<glm::vec3, 8>& corners, const glm::vec3& axis) {
    assert(glm::epsilonEqual(glm::length(axis), 1.0f, 1e-4f));

    float min = glm::dot(corners[0], axis);
    float max = min;
    for (int i = 1; i < 8; ++i) {
        float projection = glm::dot(corners[i], axis);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
    return { min, max };
}

ContactManifold collisionDetectors::findOBBOBBCollisionPoints(
    const OBBCollider* a, const Transform* transformA,
    const OBBCollider* b, const Transform* transformB
) {
    ContactManifold manifold;

    // Step 1: Compute world-space corner points of each OBB
    std::array<glm::vec3, 8> cornersA = getOBBCorners(a, transformA);
    std::array<glm::vec3, 8> cornersB = getOBBCorners(b, transformB);

    // Step 2: Extract OBB axes from transform rotation
    glm::vec3 axesA[3] = {
        glm::normalize(transformA->rotation * glm::vec3(1, 0, 0)),
        glm::normalize(transformA->rotation * glm::vec3(0, 1, 0)),
        glm::normalize(transformA->rotation * glm::vec3(0, 0, 1))
    };

    glm::vec3 axesB[3] = {
        glm::normalize(transformB->rotation * glm::vec3(1, 0, 0)),
        glm::normalize(transformB->rotation * glm::vec3(0, 1, 0)),
        glm::normalize(transformB->rotation * glm::vec3(0, 0, 1))
    };

    // Step 3: Generate the 15 potential separating axes
    std::vector<glm::vec3> testAxes;
    for (int i = 0; i < 3; ++i) testAxes.push_back(axesA[i]);
    for (int i = 0; i < 3; ++i) testAxes.push_back(axesB[i]);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            glm::vec3 axis = glm::cross(axesA[i], axesB[j]);
            if (glm::length(axis) > 0.0001f) // avoid degenerate axes
                testAxes.push_back(glm::normalize(axis));
        }

    float minOverlap = std::numeric_limits<float>::max();
    glm::vec3 smallestAxis;

    // Step 4: SAT test
    // TODO: Generalise SAT test
    for (const auto& axis : testAxes) {
        // Project both sets of corners onto the axis
        auto [minA, maxA] = projectOntoAxis(cornersA, axis);
        auto [minB, maxB] = projectOntoAxis(cornersB, axis);

        // Check for overlap
        if (maxA < minB || maxB < minA) {
            return manifold; // No collision
        }

        // Compute overlap amount
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    // Step 5: If no separating axis found, we have a collision
    manifold.normal = smallestAxis;
    manifold.depth = minOverlap;
    glm::vec3 direction = transformB->position - transformA->position;
    if (glm::dot(direction, manifold.normal) > 0.0f) manifold.normal *= -1.0f;

    bool referenceIsA = true;
    float maxDot = -1.0f;
    int referenceAxis = -1;

    // Find best matching axis from A
    for (int i = 0; i < 3; ++i) {
        float dot = glm::abs(glm::dot(axesA[i], smallestAxis));
        if (dot > maxDot) {
            maxDot = dot;
            referenceAxis = i;
            referenceIsA = true;
        }
    }

    // Find best matching axis from B
    for (int i = 0; i < 3; ++i) {
        float dot = glm::abs(glm::dot(axesB[i], smallestAxis));
        if (dot > maxDot) {
            maxDot = dot;
            referenceAxis = i;
            referenceIsA = false;
        }
    }

    const Transform* refTransform = referenceIsA ? transformA : transformB;
    const OBBCollider* refOBB = referenceIsA ? a : b;
    const glm::vec3* refAxes = referenceIsA ? axesA : axesB;

    const Transform* incTransform = referenceIsA ? transformB : transformA;
    const OBBCollider* incOBB = referenceIsA ? b : a;
    const glm::vec3* incAxes = referenceIsA ? axesB : axesA;

    glm::vec3 refCenter = refTransform->position;
    glm::vec3 refNormal = refAxes[referenceAxis];
    if (glm::dot(refNormal, smallestAxis) < 0) refNormal = -refNormal;

    // Generate incident face
    int bestIncidentAxis = 0;
    float minDot = 1.0f;
    for (int i = 0; i < 3; ++i) {
        float dot = glm::dot(incAxes[i], refNormal);
        if (dot < minDot) {
            minDot = dot;
            bestIncidentAxis = i;
        }
    }

    glm::vec3 incCenter = incTransform->position;
    glm::vec3 incNormal = incAxes[bestIncidentAxis];
    if (glm::dot(incNormal, refNormal) > 0) incNormal = -incNormal;

    glm::vec3 refRight = refAxes[(referenceAxis + 1) % 3];
    glm::vec3 refUp = refAxes[(referenceAxis + 2) % 3];

    glm::vec3 refExtent = (refOBB->maxExtents - refOBB->minExtents) * 0.5f * refTransform->scale;
    glm::vec3 centerOffset = refNormal * ((refOBB->maxExtents[referenceAxis] - refOBB->minExtents[referenceAxis]) * 0.5f * refTransform->scale[referenceAxis]);

    glm::vec3 faceCenter = refCenter + centerOffset;
    std::vector<glm::vec3> clippedPolygon;

    // Get incident face corners
    std::array<glm::vec3, 8> incCorners = getOBBCorners(incOBB, incTransform);
    for (const auto& corner : incCorners) {
        float d = glm::dot(corner - faceCenter, refNormal);
        if (d <= 0.0f) {
            clippedPolygon.push_back(corner - d * refNormal);
        }
    }

    // Use up to 4 contact points
    size_t count = std::min(size_t(4), clippedPolygon.size());
    for (size_t i = 0; i < count; ++i) {
        manifold.contactPoints[i] = clippedPolygon[i];
    }
    manifold.contactsCount = static_cast<int>(count);

    return manifold;
}

// MARK: Mixed - Plane

ContactManifold collisionDetectors::findPlaneOBBCollisionPoints(
    const BoundedPlaneCollider* a, const Transform* transformA,
    const OBBCollider* b, const Transform* transformB
) {
    ContactManifold manifold;

    // Get OBB corners in world space
    std::array<glm::vec3, 8> corners = getOBBCorners(b, transformB);

    // Transform the plane's normal to world space
    glm::vec3 planeNormal = glm::normalize(transformA->rotation * BoundedPlaneCollider::normal);

    // Compute plane's right and up vectors in world space
    glm::vec3 planeRight = glm::normalize(transformA->rotation * glm::vec3(1, 0, 0));
    glm::vec3 planeUp    = glm::normalize(transformA->rotation * glm::vec3(0, 1, 0));

    // Half extents from scale
    glm::vec2 halfExtents = glm::vec2(transformA->scale.x, transformA->scale.z);

    float maxPenetration = 0.0f;

    for (const auto& corner : corners) {
        float distance = glm::dot(corner - transformA->position, planeNormal);

        // One-sided plane: skip if in front
        if (distance > 0.0f)
            continue;

        // Project corner onto plane to check bounds
        glm::vec3 toCorner = corner - transformA->position;
        float rightDist = glm::dot(toCorner, planeRight);
        float upDist = glm::dot(toCorner, planeUp);

        if (std::abs(rightDist) > halfExtents.x || std::abs(upDist) > halfExtents.y)
            continue; // Outside bounded region

        float penetration = -distance;

        if (!manifold.contactsCount || penetration > maxPenetration) {
            maxPenetration = penetration;
        }

        if (manifold.contactsCount < 4) {
            manifold.contactPoints[manifold.contactsCount++] = corner;
        }
    }

    if (manifold.hasCollision()) {
        manifold.normal = planeNormal;
        manifold.depth = maxPenetration;
    }

    return manifold;
}

// CollisionPoints collisionDetectors::findPlaneSphereCollisionPoints(
//     const BoundedPlaneCollider* a, const Transform* transformA,
//     const SphereCollider* b, const Transform* transformB
// ) {
//     return {};
// }

// CollisionPoints collisionDetectors::findPlaneCapsuleCollisionPoints(
//     const BoundedPlaneCollider* a, const Transform* transformA,
//     const CapsuleCollider* b, const Transform* transformB
// ) {
//     CollisionPoints points;
//     points.hasCollision = false;

//     // Get capsule's world-space endpoints
//     glm::vec3 topB, bottomB;
//     b->getWorldEndpoints(transformB, topB, bottomB);

//     // Transform the plane's normal to world space
//     glm::vec3 planeNormal = glm::normalize(transformA->rotation * BoundedPlaneCollider::normal);

//     // Calculate distance of both the capsule's ends (top and bottom) from the plane
//     float topDistance = glm::dot(topB - transformA->position, planeNormal);
//     float bottomDistance = glm::dot(bottomB - transformA->position, planeNormal);

//     // Check for collision with top or bottom sphere
//     if (topDistance < b->radius && topDistance > 0.0f) {
//         points.hasCollision = true;
//         points.normal = planeNormal;
//         points.depth = b->radius - topDistance;
//         points.start = topB - planeNormal * points.depth;
//         points.end = topB;
//     }

//     if (bottomDistance < b->radius && bottomDistance > 0.0f) {
//         points.hasCollision = true;
//         points.normal = planeNormal;
//         points.depth = b->radius - bottomDistance;
//         points.start = bottomB - planeNormal * points.depth;
//         points.end = bottomB;
//     }

//     // If no collision at top or bottom, check if capsule cylinder intersects the plane
//     if (!points.hasCollision) {
//         // Check if the capsule cylinder intersects the plane, if the capsule center is above or below the plane
//         glm::vec3 capsuleCenter = (topB + bottomB) / 2.0f;
//         float centerDistance = glm::dot(capsuleCenter - transformA->position, planeNormal);

//         if (centerDistance < b->radius && centerDistance > 0.0f) {
//             points.hasCollision = true;
//             points.normal = planeNormal;
//             points.depth = b->radius - centerDistance;
//             points.start = capsuleCenter - planeNormal * points.depth;
//             points.end = capsuleCenter;
//         }
//     }

//     return points;
// }

// CollisionPoints collisionDetectors::findPlaneAABBCollisionPoints(
//     const BoundedPlaneCollider* a, const Transform* transformA,
//     const AABBCollider* b, const Transform* transformB
// ) {
//     CollisionPoints points;
//     points.hasCollision = false;

//     // Get AABB corners in world space
//     auto [aabbMin, aabbMax] = b->getAABB(transformB);

//     // Transform plane normal to world space
//     glm::vec3 planeNormal = glm::normalize(transformA->rotation * BoundedPlaneCollider::normal);

//     // Transform AABB corners into plane's local space
//     std::vector<glm::vec3> corners = {
//         // Min side
//         { aabbMin.x, aabbMin.y, aabbMin.z },
//         { aabbMax.x, aabbMin.y, aabbMin.z },
//         { aabbMin.x, aabbMax.y, aabbMin.z },
//         { aabbMin.x, aabbMin.y, aabbMax.z },
//         // Max side
//         { aabbMax.x, aabbMax.y, aabbMax.z },
//         { aabbMin.x, aabbMax.y, aabbMax.z },
//         { aabbMax.x, aabbMin.y, aabbMax.z },
//         { aabbMax.x, aabbMax.y, aabbMin.z }
//     };

//     // Project corners onto the plane
//     int behindCount = 0;
//     float minPenetration = std::numeric_limits<float>::max();
//     glm::vec3 penetrationPoint;

//     for (const auto& corner : corners) {
//         float signedDistance = glm::dot(corner - transformA->position, planeNormal);
//         if (signedDistance < 0) {
//             behindCount++;
//             if (-signedDistance < minPenetration) {
//                 minPenetration = -signedDistance;
//                 penetrationPoint = corner;
//             }
//         }
//     }

//     // If at least one corner is behind the plane, we have a collision
//     if (behindCount > 0) {
//         points.hasCollision = true;
//         points.normal = planeNormal;
//         points.depth = minPenetration;
//         points.start = penetrationPoint;
//         points.end = penetrationPoint + planeNormal * minPenetration;
//     }

//     return points;
// }