#pragma once
#include "Shapes.h"

/// @brief Checks if a capsule and a plane are colliding.
/// A capsule collides with a plane if either of its endpoints (sphere parts) 
/// are within range of the plane, or if the endpoints lie on opposite sides of the plane.
/// @param capsule The capsule to check for collision.
/// @param plane The plane to check for collision.
/// @return True if the capsule and plane are colliding, false otherwise.
bool checkCapsulePlaneCollision(const Capsule& capsule, const Plane& plane);