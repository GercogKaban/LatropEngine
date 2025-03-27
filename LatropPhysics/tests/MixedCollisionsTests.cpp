#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "collision/AABBCollider.h"
#include "collision/BoundedPlaneCollider.h"
#include "collision/algorithms/CollisionDetection.h"
#include "shared/Transform.h"

using namespace LP;
using namespace collisionDetectors;

// MARK: - Plane-AABB

TEST(MixedCollisionsTests, NoCollisionAABBPlane) {
    BoundedPlaneCollider plane;
    Transform planeTransform;
    planeTransform.position = glm::vec3(0.0f, 0.0f, 0.0f); // Plane at origin

    AABBCollider aabb(glm::vec3(-0.5f, 1.1f, -0.5f), glm::vec3(0.5f, 2.0f, 0.5f)); // Fully above the plane
    Transform aabbTransform;
    aabbTransform.position = glm::vec3(0.0f, 0.0f, 0.0f); // No extra offset

    CollisionPoints result = collisionDetectors::findPlaneAABBCollisionPoints(&plane, &planeTransform, &aabb, &aabbTransform);
    ASSERT_FALSE(result.hasCollision);
}

TEST(MixedCollisionsTests, PartialCollisionAABBPlane) {
    BoundedPlaneCollider plane;
    Transform planeTransform;
    planeTransform.position = glm::vec3(0.0f, 0.0f, 0.0f); // Plane at origin

    AABBCollider aabb(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f)); // Partly below the plane
    Transform aabbTransform;
    aabbTransform.position = glm::vec3(0.0f, 0.0f, 0.0f); // No extra offset

    CollisionPoints result = collisionDetectors::findPlaneAABBCollisionPoints(&plane, &planeTransform, &aabb, &aabbTransform);
    ASSERT_TRUE(result.hasCollision);
    ASSERT_GT(result.depth, 0.0f);
}

TEST(MixedCollisionsTests, AABBExactlyOnPlane) {
    BoundedPlaneCollider plane;
    Transform planeTransform;
    planeTransform.position = glm::vec3(0.0f, 0.0f, 0.0f); // Plane at origin

    AABBCollider aabb(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f)); // Bottom face aligned with plane
    Transform aabbTransform;
    aabbTransform.position = glm::vec3(0.0f, 0.5f, 0.0f); // No extra offset

    CollisionPoints result = collisionDetectors::findPlaneAABBCollisionPoints(&plane, &planeTransform, &aabb, &aabbTransform);
    ASSERT_FALSE(result.hasCollision);
    ASSERT_FLOAT_EQ(result.depth, 0.0f); // Should be touching, but not penetrating since we ignore this case
}