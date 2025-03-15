#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "collision/AABBCollider.h"
#include "collision/algorithms/CollisionDetection.h"

using namespace LatropPhysics;
using namespace collisionDetectors;

// MARK: 1 Test Case: No collision (separate AABBs)
TEST(AABBColliderTests, NoCollision) {
    AABBCollider a = { glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
    AABBCollider b = { glm::vec3(3, 3, 3), glm::vec3(5, 5, 5) };

    Transform identity = { glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::quat() };

    CollisionPoints result = findAABBAABBCollisionPoints(&a, &identity, &b, &identity);

    ASSERT_FALSE(result.hasCollision);
}

// MARK: 2 Test Case: Full collision (one AABB completely inside the other)
TEST(AABBColliderTests, FullOverlap) {
    AABBCollider a = { glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
    AABBCollider b = { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, 0.5) };

    Transform identity = { glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::quat() };

    CollisionPoints result = findAABBAABBCollisionPoints(&a, &identity, &b, &identity);

    ASSERT_TRUE(result.hasCollision);
    ASSERT_GT(result.depth, 0);
}

// MARK: 3 Test Case: Partial collision (AABBs overlapping partially)
TEST(AABBColliderTests, PartialOverlap) {
    AABBCollider a = { glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
    AABBCollider b = { glm::vec3(0, 0, 0), glm::vec3(2, 2, 2) };

    Transform identity = { glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::quat() };

    CollisionPoints result = findAABBAABBCollisionPoints(&a, &identity, &b, &identity);

    ASSERT_TRUE(result.hasCollision);
    ASSERT_GT(result.depth, 0);
    // Check that the normal is in the X direction
    ASSERT_EQ(result.normal.x, -1.0);
    ASSERT_EQ(result.normal.y, 0);
    ASSERT_EQ(result.normal.z, 0);
    
    // ADD_FAILURE() << "Normal: (" << result.normal.x << ", " << result.normal.y << ", " << result.normal.z << ")";
    // ASSERT_TRUE(glm::all(glm::epsilonEqual(result.normal, glm::vec3(1, 0, 0), 1e-5f)));  // Check that the normal is in the X direction
}

// MARK: 4 Test Case: AABBs are the same size and overlap
TEST(AABBColliderTests, SameSizeOverlap) {
    AABBCollider a = { glm::vec3(0, 0, 0), glm::vec3(2, 2, 2) };
    AABBCollider b = { glm::vec3(1, 1, 1), glm::vec3(3, 3, 3) };

    Transform identity = { glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::quat() };

    CollisionPoints result = findAABBAABBCollisionPoints(&a, &identity, &b, &identity);

    ASSERT_TRUE(result.hasCollision);
    ASSERT_FLOAT_EQ(result.depth, 1.0f);  // Overlap depth
    // Normal based on min-extents
    ASSERT_EQ(result.normal.x, -1.0);
    ASSERT_EQ(result.normal.y, 0);
    ASSERT_EQ(result.normal.z, 0);
}

// MARK: 5 Test Case: AABBs are the same size and touch at a corner
TEST(AABBColliderTests, CornerTouch) {
    AABBCollider a = { glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
    AABBCollider b = { glm::vec3(1, 1, 1), glm::vec3(3, 3, 3) };

    Transform identity = { glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::quat() };

    CollisionPoints result = findAABBAABBCollisionPoints(&a, &identity, &b, &identity);

    ASSERT_FALSE(result.hasCollision);
}

// MARK: 6 Test Case: AABBs collide at z-axis
TEST(AABBColliderTests, CollisionAlongZAxis) {
    AABBCollider a = { glm::vec3(0, 0, 0), glm::vec3(2, 2, 2) };
    AABBCollider b = { glm::vec3(1, 1.5, 1.75), glm::vec3(1.75, 3.25, 2.5) };

    Transform identity = { glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::quat() };

    CollisionPoints result = findAABBAABBCollisionPoints(&a, &identity, &b, &identity);

    ASSERT_TRUE(result.hasCollision);
    ASSERT_FLOAT_EQ(result.depth, 0.25f);  // Depth along Z-axis
    // Normal along Z-axis
    ASSERT_EQ(result.normal.x, 0);
    ASSERT_EQ(result.normal.y, 0);
    ASSERT_EQ(result.normal.z, -1);
}