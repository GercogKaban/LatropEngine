#include "CollisionDetection.h"
#include <glm/glm.hpp>

bool checkCapsulePlaneCollision(const Capsule& capsule, const Plane& plane) {
    float d1 = glm::dot(plane.normal, capsule.start) + plane.distance;
    float d2 = glm::dot(plane.normal, capsule.end) + plane.distance;
    return (std::abs(d1) <= capsule.radius) || (std::abs(d2) <= capsule.radius) || (d1 * d2 < 0);
}