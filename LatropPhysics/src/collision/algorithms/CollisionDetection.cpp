#include "collision/algorithms/CollisionDetection.h"
#include "collision/SphereCollider.h"
#include "collision/CapsuleCollider.h"
#include "collision/BoundedPlaneCollider.h"
#include "collision/AABBCollider.h"
#include "collision/OBBCollider.h"
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
    glm::vec3 centerA = transformA->position;
    glm::vec3 centerB = transformB->position;
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

// MARK: - Capsule

CollisionPoints collisionDetectors::findCapsuleCapsuleCollisionPoints(
    const CapsuleCollider* a, const Transform* transformA,
    const CapsuleCollider* b, const Transform* transformB
) {
   // Get world-space endpoints of both capsules
   glm::vec3 topA, bottomA, topB, bottomB;
   a->getWorldEndpoints(transformA, topA, bottomA);
   b->getWorldEndpoints(transformB, topB, bottomB);

   // Calculate the vector between the capsule centers
   glm::vec3 delta = bottomB - bottomA;
   float distance = glm::length(delta);
   float combinedRadius = a->radius + b->radius;

   CollisionPoints result;
   result.hasCollision = false;

   // Check if the distance between the capsule centers is less than the combined radii
   if (distance < combinedRadius) {
       // There is a potential collision, check further
       result.hasCollision = true;

       // Calculate normal vector between capsules
       result.normal = glm::normalize(delta);

       // Calculate the penetration depth (how far the capsules are overlapping)
       result.depth = combinedRadius - distance;

       // Calculate collision points (adjusted by radius to find exact collision)
       result.start = bottomA + result.normal * a->radius;
       result.end = bottomB - result.normal * b->radius;
   }

   return result;
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

// MARK: OBB

std::array<glm::vec3, 8> getOBBCorners(const OBBCollider* obb, const Transform* transform) {
    glm::vec3 center = (obb->minExtents + obb->maxExtents) * 0.5f;
    glm::vec3 extents = (obb->maxExtents - obb->minExtents) * 0.5f;

    glm::mat3 rot = glm::mat3(transform->rotation);

    std::array<glm::vec3, 8> corners;
    int i = 0;
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                glm::vec3 localCorner = center + glm::vec3(x * extents.x, y * extents.y, z * extents.z);
                localCorner *= transform->scale;
                corners[i++] = transform->position + rot * (localCorner - center);
            }
        }
    }

    return corners;
}

std::pair<float, float> projectOntoAxis(const std::array<glm::vec3, 8>& corners, const glm::vec3& axis) {
    float min = glm::dot(corners[0], axis);
    float max = min;
    for (int i = 1; i < 8; ++i) {
        float projection = glm::dot(corners[i], axis);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
    return { min, max };
}

CollisionPoints collisionDetectors::findOBBOBBCollisionPoints(
    const OBBCollider* a, const Transform* transformA,
    const OBBCollider* b, const Transform* transformB
) {
    CollisionPoints result;
    result.hasCollision = false;

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
    for (const auto& axis : testAxes) {
        // Project both sets of corners onto the axis
        auto [minA, maxA] = projectOntoAxis(cornersA, axis);
        auto [minB, maxB] = projectOntoAxis(cornersB, axis);

        // Check for overlap
        if (maxA < minB || maxB < minA) {
            return result; // No collision
        }

        // Compute overlap amount
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    // Step 5: If no separating axis found, we have a collision
    result.hasCollision = true;
    result.normal = smallestAxis;
    result.depth = minOverlap;
    result.start = transformA->position; // Placeholder
    result.end = transformA->position + smallestAxis * minOverlap;

    return result;
}

// MARK: Mixed - Plane

CollisionPoints collisionDetectors::findPlaneSphereCollisionPoints(
    const BoundedPlaneCollider* a, const Transform* transformA,
    const SphereCollider* b, const Transform* transformB
) {
    return {};
}

CollisionPoints collisionDetectors::findPlaneCapsuleCollisionPoints(
    const BoundedPlaneCollider* a, const Transform* transformA,
    const CapsuleCollider* b, const Transform* transformB
) {
    CollisionPoints points;
    points.hasCollision = false;

    // Get capsule's world-space endpoints
    glm::vec3 topB, bottomB;
    b->getWorldEndpoints(transformB, topB, bottomB);

    // Transform the plane's normal to world space
    glm::vec3 planeNormal = glm::normalize(transformA->rotation * BoundedPlaneCollider::normal);

    // Calculate distance of both the capsule's ends (top and bottom) from the plane
    float topDistance = glm::dot(topB - transformA->position, planeNormal);
    float bottomDistance = glm::dot(bottomB - transformA->position, planeNormal);

    // Check for collision with top or bottom sphere
    if (topDistance < b->radius && topDistance > 0.0f) {
        points.hasCollision = true;
        points.normal = planeNormal;
        points.depth = b->radius - topDistance;
        points.start = topB - planeNormal * points.depth;
        points.end = topB;
    }

    if (bottomDistance < b->radius && bottomDistance > 0.0f) {
        points.hasCollision = true;
        points.normal = planeNormal;
        points.depth = b->radius - bottomDistance;
        points.start = bottomB - planeNormal * points.depth;
        points.end = bottomB;
    }

    // If no collision at top or bottom, check if capsule cylinder intersects the plane
    if (!points.hasCollision) {
        // Check if the capsule cylinder intersects the plane, if the capsule center is above or below the plane
        glm::vec3 capsuleCenter = (topB + bottomB) / 2.0f;
        float centerDistance = glm::dot(capsuleCenter - transformA->position, planeNormal);

        if (centerDistance < b->radius && centerDistance > 0.0f) {
            points.hasCollision = true;
            points.normal = planeNormal;
            points.depth = b->radius - centerDistance;
            points.start = capsuleCenter - planeNormal * points.depth;
            points.end = capsuleCenter;
        }
    }

    return points;
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
    glm::vec3 planeNormal = glm::normalize(transformA->rotation * BoundedPlaneCollider::normal);

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

// MARK: Mixed - AABB

// Function to compute the squared distance between two points
float distanceSquaredBetween(const glm::vec3& a, const glm::vec3& b) {
    return glm::dot(a - b, a - b);
}

// Function to compute the closest point on the capsule to a point
glm::vec3 closestPointOnSegment(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b) {
    glm::vec3 ab = b - a;
    float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
    t = glm::clamp(t, 0.0f, 1.0f);  // Clamp t to the segment [0, 1]
    return a + t * ab;
}

CollisionPoints collisionDetectors::findAABBCapsuleCollisionPoints(
    const AABBCollider* a, const Transform* transformA,
    const CapsuleCollider* capsule, const Transform* transformB
) {
    // CollisionPoints points;
    // points.hasCollision = false;

    // // Get AABB of object A
    // auto [aMin, aMax] = a->getAABB(transformA);

    // // Get capsule position and radius
    // glm::vec3 capsuleP1, capsuleP2;
    // float capsuleRadius = capsule->radius;
    // capsule->getWorldEndpoints(transformB, capsuleP2, capsuleP1);

    // // List of 8 corners of the AABB
    // std::array<glm::vec3, 8> corners = {
    //     glm::vec3(aMin.x, aMin.y, aMin.z),
    //     glm::vec3(aMax.x, aMin.y, aMin.z),
    //     glm::vec3(aMin.x, aMax.y, aMin.z),
    //     glm::vec3(aMax.x, aMax.y, aMin.z),
    //     glm::vec3(aMin.x, aMin.y, aMax.z),
    //     glm::vec3(aMax.x, aMin.y, aMax.z),
    //     glm::vec3(aMin.x, aMax.y, aMax.z),
    //     glm::vec3(aMax.x, aMax.y, aMax.z)
    // };

    // // Iterate over each AABB face (center of each face)
    // for (int i = 0; i < 6; ++i) {
    //     glm::vec3 faceCenter;
    //     if (i == 0) faceCenter = glm::vec3((aMin.x + aMax.x) / 2, aMin.y, (aMin.z + aMax.z) / 2); // Bottom
    //     else if (i == 1) faceCenter = glm::vec3((aMin.x + aMax.x) / 2, aMax.y, (aMin.z + aMax.z) / 2); // Top
    //     else if (i == 2) faceCenter = glm::vec3(aMin.x, (aMin.y + aMax.y) / 2, (aMin.z + aMax.z) / 2); // Left
    //     else if (i == 3) faceCenter = glm::vec3(aMax.x, (aMin.y + aMax.y) / 2, (aMin.z + aMax.z) / 2); // Right
    //     else if (i == 4) faceCenter = glm::vec3((aMin.x + aMax.x) / 2, (aMin.y + aMax.y) / 2, aMin.z); // Front
    //     else faceCenter = glm::vec3((aMin.x + aMax.x) / 2, (aMin.y + aMax.y) / 2, aMax.z); // Back

    //     // Find the closest point on the capsule segment to this face center
    //     glm::vec3 closestPointOnCapsule = closestPointOnSegment(faceCenter, capsuleP1, capsuleP2);
        
    //     // Check distance to the capsule and detect collision
    //     float distanceSquared = distanceSquaredBetween(closestPointOnCapsule, faceCenter);
    //     if (distanceSquared <= capsuleRadius * capsuleRadius) {
    //         points.hasCollision = true;

    //         float penetrationDepth = capsuleRadius - std::sqrt(distanceSquared);
    //         glm::vec3 normal = glm::normalize(closestPointOnCapsule - faceCenter);

    //         if (points.depth == 0.0f || penetrationDepth < points.depth) {
    //             points.depth = penetrationDepth;
    //             points.normal = normal;
    //             points.start = faceCenter;
    //             points.end = closestPointOnCapsule;
    //         }
    //     }
    // }

    // // Iterate over each AABB edge (each pair of corners)
    // std::array<std::pair<glm::vec3, glm::vec3>, 12> edges = {
    //     std::make_pair(corners[0], corners[1]),
    //     std::make_pair(corners[1], corners[3]),
    //     std::make_pair(corners[3], corners[2]),
    //     std::make_pair(corners[2], corners[0]),
    //     std::make_pair(corners[4], corners[5]),
    //     std::make_pair(corners[5], corners[7]),
    //     std::make_pair(corners[7], corners[6]),
    //     std::make_pair(corners[6], corners[4]),
    //     std::make_pair(corners[0], corners[4]),
    //     std::make_pair(corners[1], corners[5]),
    //     std::make_pair(corners[3], corners[7]),
    //     std::make_pair(corners[2], corners[6])
    // };

    // for (const auto& edge : edges) {
    //     // Find the closest point on the capsule's segment to the edge's line segment
    //     glm::vec3 closestPointOnCapsule = closestPointOnSegment(edge.first, capsuleP1, capsuleP2);
        
    //     // Check distance to the capsule and detect collision
    //     float distanceSquared = distanceSquaredBetween(closestPointOnCapsule, edge.first);
    //     if (distanceSquared <= capsuleRadius * capsuleRadius) {
    //         points.hasCollision = true;

    //         float penetrationDepth = capsuleRadius - std::sqrt(distanceSquared);
    //         glm::vec3 normal = glm::normalize(closestPointOnCapsule - edge.first);

    //         if (points.depth == 0.0f || penetrationDepth < points.depth) {
    //             points.depth = penetrationDepth;
    //             points.normal = normal;
    //             points.start = edge.first;
    //             points.end = closestPointOnCapsule;
    //         }
    //     }
    // }

    return {};
}

// CollisionPoints collisionDetectors::findAABBCapsuleCollisionPoints(
//     const AABBCollider* a, const Transform* transformA,
//     const CapsuleCollider* capsule, const Transform* transformB
// ) {
//     CollisionPoints points;
//     points.hasCollision = false;

//     // Get AABB of object A (same as before)
//     auto [aMin, aMax] = a->getAABB(transformA);

//     // Get capsule position and radius (assumed capsule has p1, p2, radius)
//     glm::vec3 capsuleP1, capsuleP2;
//     float capsuleRadius = capsule->radius;
//     capsule->getWorldEndpoints(transformB, capsuleP2, capsuleP1);

//     // Iterate over all 8 corners of the AABB
//     std::array<glm::vec3, 8> corners = {
//         glm::vec3(aMin.x, aMin.y, aMin.z),
//         glm::vec3(aMax.x, aMin.y, aMin.z),
//         glm::vec3(aMin.x, aMax.y, aMin.z),
//         glm::vec3(aMax.x, aMax.y, aMin.z),
//         glm::vec3(aMin.x, aMin.y, aMax.z),
//         glm::vec3(aMax.x, aMin.y, aMax.z),
//         glm::vec3(aMin.x, aMax.y, aMax.z),
//         glm::vec3(aMax.x, aMax.y, aMax.z)
//     };

//     // Check for closest point on the capsule for each corner
//     for (const auto& corner : corners) {
//         // Find the closest point on the capsule's segment to the corner
//         glm::vec3 closestPointOnCapsule = closestPointOnSegment(corner, capsuleP1, capsuleP2);

//         // Calculate squared distance from the corner to the closest point on the capsule
//         float distanceSquaredToCapsule = distanceSquared(closestPointOnCapsule, corner);

//         // Check if this distance is within the capsule's radius
//         if (distanceSquaredToCapsule <= capsuleRadius * capsuleRadius) {
//             points.hasCollision = true;

//             // Determine the penetration depth (the distance to the capsule's surface)
//             float penetrationDepth = capsuleRadius - std::sqrt(distanceSquaredToCapsule);

//             // Calculate the normal (pointing towards the closest point on the capsule)
//             glm::vec3 normal = glm::normalize(closestPointOnCapsule - corner);

//             // If this is the first collision or if this penetration depth is smaller than previous
//             if (points.depth == 0.0f || penetrationDepth < points.depth) {
//                 points.depth = penetrationDepth;
//                 points.normal = normal;
//                 points.start = corner;
//                 points.end = closestPointOnCapsule;
//             }
//         }
//     }

//     return points;
// }