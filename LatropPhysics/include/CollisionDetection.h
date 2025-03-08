#pragma once
#include "Shapes.h"

/// @brief Check if a capsule and a plane are colliding.
/// A capsule collides with a plane if either of its endpoints (sphere parts) 
/// are within range.
/// @param capsule The capsule to check.
/// @param plane The plane to check.
/// @return True if the capsule and plane are colliding, false otherwise.
bool checkCapsulePlaneCollision(const Capsule& capsule, const Plane& plane);