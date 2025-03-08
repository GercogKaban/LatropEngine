#include "CollisionDetection.h"
#include <glm/glm.hpp>

bool checkCapsulePlaneCollision(const Capsule& capsule, const Plane& plane) 
{
    // Compute distances of the capsule's endpoints from the plane
    float d1 = glm::dot(capsule.start, plane.normal) - plane.distance;
    float d2 = glm::dot(capsule.end, plane.normal) - plane.distance;
    
    // The capsule intersects the plane if either endpoint is inside the plane
    // or if the segment crosses the plane.
    return (d1 * d2 <= 0) || (std::abs(d1) <= capsule.radius) || (std::abs(d2) <= capsule.radius);
}