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
    EXPECT_TRUE(Vec3Equal(manifold.contactPoints[0].location, { 0.4f, 0.495f, -0.5f }));
    EXPECT_TRUE(Vec3Equal(manifold.contactPoints[1].location, { 0.4f, 0.495f, 0.5f }));
    EXPECT_TRUE(Vec3Equal(manifold.contactPoints[2].location, { 0.5f, 0.5f,  -0.5f }));
    EXPECT_TRUE(Vec3Equal(manifold.contactPoints[3].location, { 0.5f, 0.5f,  0.5f }));
}