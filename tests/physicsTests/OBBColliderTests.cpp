#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "collision/OBBCollider.h"
#include "shared/Transform.h"
#include "Utilities.h"

using namespace LP;

// MARK: 1 Test Case: No collision (separate OBBs)
TEST(OBBColliderTests, NoCollision) {
    OBBCollider cubeA = OBBCollider::makeCube();
    OBBCollider cubeB = OBBCollider::makeCube();
    
    Transform transformA, transformB;
    transformB.position = glm::vec3(0.9f, 5.0f, 0.0f);

    ContactManifold manifold = cubeA.testCollision(&transformA, &cubeB, &transformB);

    ASSERT_EQ(manifold.contactsCount, 0);
}

TEST(OBBColliderTests, ATopCollision4Points) {
    OBBCollider cubeA = OBBCollider::makeCube();
    OBBCollider cubeB = OBBCollider::makeCube();
    
    Transform transformA, transformB;
    transformB.position = glm::vec3(0.9f, 0.995f, 0.0f);

    ContactManifold manifold = cubeA.testCollision(&transformA, &cubeB, &transformB);

    ASSERT_EQ(manifold.contactsCount, 4);
    EXPECT_NEAR(manifold.depth, 0.004999f, 0.000001f);

    // The contacts are the corners of the overlap patch (x in [0.4, 0.5]),
    // halfway between the two surfaces (0.495 and 0.5). Their order is not defined.
    const glm::vec3 expected[4] = {
        { 0.4f, 0.4975f, -0.5f }, { 0.4f, 0.4975f, 0.5f },
        { 0.5f, 0.4975f, -0.5f }, { 0.5f, 0.4975f, 0.5f }
    };

    for (const glm::vec3& point : expected)
    {
        bool found = false;
        for (uint32_t i = 0; i < manifold.contactsCount; ++i)
        {
            found = found || glm::all(glm::lessThan(glm::abs(manifold.contactPoints[i] - point), glm::vec3(1e-4f)));
        }
        EXPECT_TRUE(found) << "Missing contact point (" << point.x << ", " << point.y << ", " << point.z << ")";
    }
}

TEST(OBBColliderTests, ATopContact4Points) {
    OBBCollider cubeA = OBBCollider::makeCube();
    OBBCollider cubeB = OBBCollider::makeCube();
    
    Transform transformA, transformB;
    transformB.position = glm::vec3(0.9f, 0.995f, 0.0f);

    ContactManifold manifold = cubeA.testCollision(&transformA, &cubeB, &transformB);

    ASSERT_EQ(manifold.contactsCount, 4);
    EXPECT_NEAR(manifold.depth, 0.0f, 0.1f);
}