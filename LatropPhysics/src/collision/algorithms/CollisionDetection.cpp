#include "../include/collision/algorithms/CollisionDetection.h"

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

CollisionPoints collisionDetectors::findPlaneSphereCollisionPoints(
    const PlaneCollider* a, const Transform* transformA,
    const SphereCollider* b, const Transform* transformB
) {
    return findSpherePlaneCollisionPoints(b, transformB, a, transformA);
}