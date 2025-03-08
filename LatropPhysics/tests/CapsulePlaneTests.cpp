#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "CollisionDetection.h"

// The capsule starts below the plane and ends above it, crossing 
// through the plane. 
// |
// |      O   <- Capsule end
// |      
// |----- (Plane at y = 0) -----
// |
// |      O   <- Capsule start
// |
TEST(CapsulePlaneTests, CapsuleIntersectsPlane)
{
    // Define a horizontal plane at y = 0 with an upward normal.
    Plane plane = {glm::vec3(0, 1, 0), 0};
    
    // Define a capsule that starts below the plane and ends above it.
    Capsule capsule = {glm::vec3(0, -1, 0), glm::vec3(0, 1, 0), 0.5f};
    
    // Since the capsule's segment crosses the plane, it should return true.
    ASSERT_TRUE(checkCapsulePlaneCollision(capsule, plane));
}
 
// The capsule is entirely above the plane and does not touch it.
// |
// |      O   <- Capsule start
// |      |   
// |      O   <- Capsule end
// |      
// |----- (Plane at y = 0) -----
// |
TEST(CapsulePlaneTests, CapsuleAbovePlane)
{
    // Define a horizontal plane at y = 0.
    Plane plane = {glm::vec3(0, 1, 0), 0};
    
    // Define a capsule that is fully above the plane.
    Capsule capsule = {glm::vec3(0, 2, 0), glm::vec3(0, 3, 0), 0.5f};
    
    // Since the capsule is entirely above and does not touch the plane, it should return false.
    ASSERT_FALSE(checkCapsulePlaneCollision(capsule, plane));
}

// The capsule’s endpoint is above the plane, but its radius extends below, 
// touching the plane.
// |
// |      O   <- Capsule end
// |      
// |----- (Plane at y = 0) -----
// |      ()  <- Capsule radius touches the plane
// |      O   <- Capsule start
// |
TEST(CapsulePlaneTests, CapsuleTouchingPlane)
{
    // Define a horizontal plane at y = 0.
    Plane plane = {glm::vec3(0, 1, 0), 0};
    
    // Define a capsule that has one endpoint on the plane and extends upward.
    Capsule capsule = {glm::vec3(0, 1, 0), glm::vec3(0, 2, 0), 1.0f};
    
    // The capsule has a radius of 1.0f, meaning it extends below y = 0, touching the plane.
    // This means it should return true.
    ASSERT_TRUE(checkCapsulePlaneCollision(capsule, plane));
}

// The capsule starts well below the plane and ends well above, fully crossing it.
// |
// |      O   <- Capsule end
// |      
// |----- (Plane at y = 0) -----
// |
// |      O   <- Capsule start
// |
TEST(CapsulePlaneTests, CapsuleCrossingPlane)
{
    // Define a horizontal plane at y = 0.
    Plane plane = {glm::vec3(0, 1, 0), 0};
    
    // Define a capsule that starts below the plane and ends above it.
    Capsule capsule = {glm::vec3(0, -2, 0), glm::vec3(0, 2, 0), 0.5f};
    
    // Since the capsule's segment clearly crosses the plane, it should return true.
    ASSERT_TRUE(checkCapsulePlaneCollision(capsule, plane));
}

// 1. The plane is horizontal at y = 0.
// 2. The capsule starts on the left (-1, 0, 0) and ends on the right (1, 0, 0).
// •	The segment itself is inside the plane (y = 0).
// •	The capsule has a radius that extends above and below y = 0.
// Thus, they should intersect.
//        Z
//        ↑
//        |
//  O─────O   <- Capsule segment lying on the plane at y = 0
// ──────────  (Plane at y = 0)
//        |
//        X →
TEST(CapsulePlaneTests, CapsuleHorizontallyLyingOnPlane)
{
    // Define a horizontal plane at y = 0.
    Plane plane = {glm::vec3(0, 1, 0), 0}; // Normal pointing upwards

    // Define a capsule that lies flat along the x-axis, exactly at y = 0.
    Capsule capsule = {glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0), 0.5f};

    // The segment itself is in the plane, and the radius extends into it.
    // This means it should return true.
    ASSERT_TRUE(checkCapsulePlaneCollision(capsule, plane));
}

// 1. The plane is horizontal at y = 0.
// 2. The capsule starts at the bottom (0, 0, 0) touching the plane and ends
// at the top (0, 2, 0) standing upright with a radius of 0.5.
// The capsule intersects the plane because its bottom is exactly on it, 
// and the radius extends into it.
//        Z
//        ↑
//        |
//        O   <- Capsule top (0,2,0)
//        |
//        |
//        O   <- Capsule bottom (0,0,0) touching the plane
// ──────────  <- Plane at y = 0
//        |
//        X →
TEST(CapsulePlaneTests, CapsuleVerticalLyingOnPlane)
{
    // Define a horizontal plane at y = 0.
    Plane plane = {glm::vec3(0, 1, 0), 0};
    
    // Define a vertical capsule with its bottom endpoint touching the plane at y = 0.
    Capsule capsule = {glm::vec3(0, 0, 0), glm::vec3(0, 2, 0), 0.5f};
    
    // The bottom of the capsule is exactly on the plane, and its radius extends into it.
    // This means it should return true.
    ASSERT_TRUE(checkCapsulePlaneCollision(capsule, plane));
}