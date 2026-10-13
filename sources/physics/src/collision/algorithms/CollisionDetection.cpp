#include <vector>
#include <limits>
#include <algorithm>

#include "collision/algorithms/CollisionDetection.h"
#include "collision/SphereCollider.h"
#include "collision/CapsuleCollider.h"
#include "collision/BoundedPlaneCollider.h"
#include "collision/OBBCollider.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

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
    if (overlapMin.x >= overlapMax.x || overlapMin.y >= overlapMax.y || overlapMin.z >= overlapMax.z) 
    {
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

std::array<glm::vec3, 8> getOBBCorners(const OBBCollider* obb, const Transform* transform) 
{
    glm::vec3 extents = (obb->maxExtents - obb->minExtents) * 0.5f;

    glm::mat3 rot = glm::mat3_cast(transform->rotation);

    assert(glm::length(transform->scale) > 0.0f);

    std::array<glm::vec3, 8> corners;
    int i = 0;
    for (int x = -1; x <= 1; x += 2) 
    {
        for (int y = -1; y <= 1; y += 2)
        {
            for (int z = -1; z <= 1; z += 2) 
            {
                glm::vec3 offset = glm::vec3(x * extents.x, y * extents.y, z * extents.z);
                glm::vec3 scaledOffset = offset * transform->scale;
                corners[i++] = transform->position + rot * scaledOffset;
            }
        }
    }

    return corners;
}

namespace
{
    /// Contact points that are further than this above the reference face are
    /// dropped. A resting box is never perfectly flat, so without some margin
    /// the raised corners disappear and the manifold becomes one-sided, which
    /// makes the solver spin the box instead of holding it.
    constexpr float contactMargin = 0.02f;

    /// Edge axes must win over face axes by this much to be selected. Parallel
    /// edges of nearly aligned boxes produce noisy cross products that would
    /// otherwise beat the (correct) face axis and push the boxes sideways.
    constexpr float edgeAxisRelativeBias = 0.95f;
    constexpr float edgeAxisAbsoluteBias = 0.001f;

    struct OrientedBox
    {
        glm::vec3 center;
        glm::vec3 axes[3];
        glm::vec3 halfExtents;
    };

    OrientedBox makeOrientedBox(const OBBCollider* obb, const Transform* transform)
    {
        OrientedBox box;
        box.center = transform->position;
        box.halfExtents = (obb->maxExtents - obb->minExtents) * 0.5f * glm::abs(transform->scale);
        for (int i = 0; i < 3; ++i)
        {
            glm::vec3 unit(0.0f);
            unit[i] = 1.0f;
            box.axes[i] = glm::normalize(transform->rotation * unit);
        }
        return box;
    }

    /// Distance from the box center to its surface along the given direction.
    float projectedRadius(const OrientedBox& box, const glm::vec3& direction)
    {
        return box.halfExtents.x * std::abs(glm::dot(direction, box.axes[0]))
             + box.halfExtents.y * std::abs(glm::dot(direction, box.axes[1]))
             + box.halfExtents.z * std::abs(glm::dot(direction, box.axes[2]));
    }

    /// Sutherland-Hodgman: keeps the part of the polygon with dot(p, normal) <= offset.
    std::vector<glm::vec3> clipPolygon(
        const std::vector<glm::vec3>& polygon,
        const glm::vec3& normal,
        float offset
    ) {
        std::vector<glm::vec3> result;
        if (polygon.empty()) return result;

        glm::vec3 previous = polygon.back();
        float previousDistance = glm::dot(previous, normal) - offset;

        for (const glm::vec3& current : polygon)
        {
            float currentDistance = glm::dot(current, normal) - offset;

            if ((currentDistance <= 0.0f) != (previousDistance <= 0.0f))
            {
                float t = previousDistance / (previousDistance - currentDistance);
                result.push_back(previous + t * (current - previous));
            }
            if (currentDistance <= 0.0f)
            {
                result.push_back(current);
            }

            previous = current;
            previousDistance = currentDistance;
        }

        return result;
    }

    /// Picks the 4 points that span the largest area, always keeping the deepest one.
    std::vector<size_t> reduceContacts(const std::vector<glm::vec3>& points, const std::vector<float>& penetrations, const glm::vec3& normal)
    {
        std::vector<size_t> all(points.size());
        for (size_t i = 0; i < all.size(); ++i) all[i] = i;
        if (points.size() <= ContactManifold::maxContactsCountIn3D) return all;

        size_t first = 0;
        for (size_t i = 1; i < points.size(); ++i)
            if (penetrations[i] > penetrations[first]) first = i;

        size_t second = first;
        float farthest = -1.0f;
        for (size_t i = 0; i < points.size(); ++i)
        {
            float d = glm::dot(points[i] - points[first], points[i] - points[first]);
            if (d > farthest) { farthest = d; second = i; }
        }

        size_t third = first, fourth = first;
        float maxArea = 0.0f, minArea = 0.0f;
        for (size_t i = 0; i < points.size(); ++i)
        {
            float area = glm::dot(glm::cross(points[second] - points[first], points[i] - points[first]), normal);
            if (area > maxArea) { maxArea = area; third = i; }
            if (area < minArea) { minArea = area; fourth = i; }
        }

        std::vector<size_t> chosen = { first, second };
        if (third != first) chosen.push_back(third);
        if (fourth != first && fourth != third) chosen.push_back(fourth);
        return chosen;
    }
}

ContactManifold collisionDetectors::findOBBOBBCollisionPoints(
    const OBBCollider* a, const Transform* transformA,
    const OBBCollider* b, const Transform* transformB
) {
    ContactManifold manifold;

    const OrientedBox boxA = makeOrientedBox(a, transformA);
    const OrientedBox boxB = makeOrientedBox(b, transformB);
    const glm::vec3 delta = boxB.center - boxA.center;

    // Step 1: SAT. Face axes and edge axes are tracked separately so that face
    // contacts can be preferred, see edgeAxisRelativeBias.
    float faceOverlap = std::numeric_limits<float>::max();
    int faceOwner = 0;     // 0 - A, 1 - B
    int faceIndex = 0;

    auto overlapAlong = [&](const glm::vec3& axis)
    {
        return projectedRadius(boxA, axis) + projectedRadius(boxB, axis) - std::abs(glm::dot(delta, axis));
    };

    for (int i = 0; i < 3; ++i)
    {
        float overlap = overlapAlong(boxA.axes[i]);
        if (overlap < 0.0f) return manifold;
        if (overlap < faceOverlap) { faceOverlap = overlap; faceOwner = 0; faceIndex = i; }
    }

    for (int i = 0; i < 3; ++i)
    {
        float overlap = overlapAlong(boxB.axes[i]);
        if (overlap < 0.0f) return manifold;
        // Prefer A as the reference box when both are about equal, otherwise
        // the reference flips between frames and so do the contact points.
        if (overlap < faceOverlap * 0.98f - 0.001f) { faceOverlap = overlap; faceOwner = 1; faceIndex = i; }
    }

    float edgeOverlap = std::numeric_limits<float>::max();
    int edgeI = 0, edgeJ = 0;
    glm::vec3 edgeAxis(0.0f);

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            glm::vec3 axis = glm::cross(boxA.axes[i], boxB.axes[j]);
            float length = glm::length(axis);
            if (length < 1e-4f) continue; // parallel edges

            axis /= length;
            float overlap = overlapAlong(axis);
            if (overlap < 0.0f) return manifold;
            if (overlap < edgeOverlap) { edgeOverlap = overlap; edgeI = i; edgeJ = j; edgeAxis = axis; }
        }
    }

    // Step 2: Contact generation
    if (edgeOverlap < faceOverlap * edgeAxisRelativeBias - edgeAxisAbsoluteBias)
    {
        // Edge - edge: a single contact between the closest points of the two edges.
        glm::vec3 axisAtoB = glm::dot(delta, edgeAxis) >= 0.0f ? edgeAxis : -edgeAxis;

        glm::vec3 pointA = boxA.center;
        glm::vec3 pointB = boxB.center;
        for (int k = 0; k < 3; ++k)
        {
            if (k != edgeI) pointA += (glm::dot(axisAtoB, boxA.axes[k]) >= 0.0f ? 1.0f : -1.0f) * boxA.halfExtents[k] * boxA.axes[k];
            if (k != edgeJ) pointB -= (glm::dot(axisAtoB, boxB.axes[k]) >= 0.0f ? 1.0f : -1.0f) * boxB.halfExtents[k] * boxB.axes[k];
        }

        const glm::vec3& dirA = boxA.axes[edgeI];
        const glm::vec3& dirB = boxB.axes[edgeJ];
        glm::vec3 r = pointA - pointB;
        float bb = glm::dot(dirA, dirB);
        float c = glm::dot(dirA, r);
        float f = glm::dot(dirB, r);
        float denom = 1.0f - bb * bb;

        float s = (bb * f - c) / denom;
        float t = bb * s + f;

        manifold.contactPoints[0] = 0.5f * ((pointA + s * dirA) + (pointB + t * dirB));
        manifold.penetrations[0] = edgeOverlap;
        manifold.contactsCount = 1;
        manifold.depth = edgeOverlap;
        manifold.normal = -axisAtoB; // Points from B to A
        return manifold;
    }

    // Face contact: clip the incident face against the side planes of the reference face.
    const OrientedBox& reference = faceOwner == 0 ? boxA : boxB;
    const OrientedBox& incident  = faceOwner == 0 ? boxB : boxA;

    // Normal of the reference face pointing towards the incident box.
    glm::vec3 referenceDelta = incident.center - reference.center;
    glm::vec3 referenceNormal = reference.axes[faceIndex];
    if (glm::dot(referenceDelta, referenceNormal) < 0.0f) referenceNormal = -referenceNormal;

    // Incident face is the one that faces the reference box the most.
    int incidentIndex = 0;
    float mostAntiParallel = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        float d = std::abs(glm::dot(referenceNormal, incident.axes[i]));
        if (d > mostAntiParallel) { mostAntiParallel = d; incidentIndex = i; }
    }

    glm::vec3 incidentNormal = incident.axes[incidentIndex];
    if (glm::dot(incidentNormal, referenceNormal) > 0.0f) incidentNormal = -incidentNormal;

    int u = (incidentIndex + 1) % 3;
    int v = (incidentIndex + 2) % 3;
    glm::vec3 incidentCenter = incident.center + incidentNormal * incident.halfExtents[incidentIndex];
    glm::vec3 du = incident.axes[u] * incident.halfExtents[u];
    glm::vec3 dv = incident.axes[v] * incident.halfExtents[v];

    std::vector<glm::vec3> polygon = {
        incidentCenter + du + dv,
        incidentCenter - du + dv,
        incidentCenter - du - dv,
        incidentCenter + du - dv
    };

    for (int k = 0; k < 3 && !polygon.empty(); ++k)
    {
        if (k == faceIndex) continue;
        const glm::vec3& side = reference.axes[k];
        float offset = glm::dot(reference.center, side);
        polygon = clipPolygon(polygon, side, offset + reference.halfExtents[k]);
        polygon = clipPolygon(polygon, -side, -offset + reference.halfExtents[k]);
    }

    float referenceOffset = glm::dot(reference.center, referenceNormal) + reference.halfExtents[faceIndex];

    std::vector<glm::vec3> points;
    std::vector<float> penetrations;
    for (const glm::vec3& point : polygon)
    {
        float separation = glm::dot(point, referenceNormal) - referenceOffset;
        if (separation > contactMargin) continue;

        // Report the point halfway between the two surfaces.
        points.push_back(point - referenceNormal * (separation * 0.5f));
        penetrations.push_back(-separation);
    }

    if (points.empty()) return manifold;

    std::vector<size_t> chosen = reduceContacts(points, penetrations, referenceNormal);
    float deepest = 0.0f;
    for (size_t i = 0; i < chosen.size(); ++i)
    {
        manifold.contactPoints[i] = points[chosen[i]];
        manifold.penetrations[i] = penetrations[chosen[i]];
        deepest = std::max(deepest, penetrations[chosen[i]]);
    }

    manifold.contactsCount = static_cast<uint32_t>(chosen.size());
    manifold.depth = deepest;

    // Normal points from B to A. referenceNormal points from the reference box to the other one.
    manifold.normal = faceOwner == 0 ? -referenceNormal : referenceNormal;

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

    for (const auto& corner : corners) 
    {
        float distance = glm::dot(corner - transformA->position, planeNormal);

        // Skip if in front or too far behind (outside bounded depth)
        if (distance > 0.0f || distance < -transformA->scale.y)
            continue;

        // Project corner onto plane to check bounds
        glm::vec3 toCorner = corner - transformA->position;
        float rightDist = glm::dot(toCorner, planeRight);
        float upDist = glm::dot(toCorner, planeUp);

        if (std::abs(rightDist) > halfExtents.x || std::abs(upDist) > halfExtents.y)
            continue; // Outside bounded region

        float penetration = -distance;

        if (!manifold.contactsCount || penetration > maxPenetration) 
        {
            maxPenetration = penetration;
        }

        if (manifold.contactsCount < ContactManifold::maxContactsCountIn3D) 
        {
            manifold.penetrations[manifold.contactsCount] = penetration;
            manifold.contactPoints[manifold.contactsCount++] = corner;
        }
    }

    if (manifold.hasCollision()) 
    {
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