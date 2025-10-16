#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"
#include "shared/AABB.h"

using namespace LP;
using namespace collisionDetectors;

// MARK: 1 Test Case: No collision (separate AABBs)
TEST(AABBColliderTests, NoCollision) {
    AABB a = { glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
    AABB b = { glm::vec3(3, 3, 3), glm::vec3(5, 5, 5) };

    bool hasCollision = testAABBAABBForCollision(a, b);

    ASSERT_FALSE(hasCollision);
}

// MARK: 2 Test Case: Full collision (one AABB completely inside the other)
TEST(AABBColliderTests, FullOverlap) {
    AABB a = { glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
    AABB b = { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, 0.5) };

    bool hasCollision = testAABBAABBForCollision(a, b);

    ASSERT_TRUE(hasCollision);
}

// MARK: 3 Test Case: Partial collision (AABBs overlapping partially)
TEST(AABBColliderTests, PartialOverlap) {
    AABB a = { glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
    AABB b = { glm::vec3(0, 0, 0), glm::vec3(2, 2, 2) };

    bool hasCollision = testAABBAABBForCollision(a, b);

    ASSERT_TRUE(hasCollision);
}

// MARK: 4 Test Case: AABBs are the same size and overlap
TEST(AABBColliderTests, SameSizeOverlap) {
    AABB a = { glm::vec3(0, 0, 0), glm::vec3(2, 2, 2) };
    AABB b = { glm::vec3(1, 1, 1), glm::vec3(3, 3, 3) };

    bool hasCollision = testAABBAABBForCollision(a, b);

    ASSERT_TRUE(hasCollision);
}

// MARK: 5 Test Case: AABBs are the same size and touch at a corner
TEST(AABBColliderTests, CornerTouch) {
    AABB a = { glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
    AABB b = { glm::vec3(1, 1, 1), glm::vec3(3, 3, 3) };

    bool hasCollision = testAABBAABBForCollision(a, b);

    ASSERT_FALSE(hasCollision);
}

// MARK: 6 Test Case: AABBs collide at z-axis
TEST(AABBColliderTests, CollisionAlongZAxis) {
    AABB a = { glm::vec3(0, 0, 0), glm::vec3(2, 2, 2) };
    AABB b = { glm::vec3(1, 1.5, 1.75), glm::vec3(1.75, 3.25, 2.5) };

    bool hasCollision = testAABBAABBForCollision(a, b);

    ASSERT_TRUE(hasCollision);
}