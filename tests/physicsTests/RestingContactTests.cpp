#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include "dynamics/DynamicsWorld.h"
#include "dynamics/RotationalImpulseSolver.h"
#include "dynamics/SmoothPositionSolver.h"
#include "collision/OBBCollider.h"

using namespace LP;

namespace
{
    constexpr float fixedDelta = 0.001f; // Same as the engine.

    struct RestingWorld
    {
        RestingWorld()
        {
            world.addSolver(std::make_unique<RotationalImpulseSolver>());
            world.addSolver(std::make_unique<SmoothPositionSolver>());

            auto floor = std::make_shared<RigidBody>();
            floor->setIsSimulated(false);
            floor->collider = collider;
            floor->transform.scale = { 5.0f, 1.0f, 5.0f };
            floor->material = Material::Metal;
            world.addRigidBody(floor);
            world.updateSpacialPartitioningOfStaticBodies(1.0f);
            bodies.push_back(floor);
        }

        std::shared_ptr<RigidBody> addCube(float mass, glm::vec3 position, Material material = Material::Wood)
        {
            auto cube = std::make_shared<RigidBody>();
            cube->setIsSimulated(true);
            cube->setMass(mass);
            cube->takesGravity = true;
            cube->collider = collider;
            cube->transform.position = position;
            cube->material = material;
            world.addRigidBody(cube);
            bodies.push_back(cube);
            return cube;
        }

        /// Runs the simulation and reports how much the body moved during the last `watchSeconds`.
        struct Motion { float maxAngularSpeed = 0.0f; float heightRange = 0.0f; float horizontalDrift = 0.0f; };

        Motion run(const std::shared_ptr<RigidBody>& watched, float seconds, float watchSeconds)
        {
            Motion motion;
            float minY = 1e9f, maxY = -1e9f;
            glm::vec3 startPosition(0.0f);

            const int steps = int(seconds / fixedDelta);
            const int watchFrom = steps - int(watchSeconds / fixedDelta);

            for (int i = 0; i < steps; ++i)
            {
                world.integrate(fixedDelta);
                if (i == watchFrom) startPosition = watched->transform.position;
                if (i >= watchFrom)
                {
                    motion.maxAngularSpeed = std::max(motion.maxAngularSpeed, glm::length(watched->angularVelocity));
                    minY = std::min(minY, watched->transform.position.y);
                    maxY = std::max(maxY, watched->transform.position.y);
                }
            }

            glm::vec3 drift = watched->transform.position - startPosition;
            motion.heightRange = maxY - minY;
            motion.horizontalDrift = glm::length(glm::vec2(drift.x, drift.z));
            return motion;
        }

        std::shared_ptr<OBBCollider> collider = std::make_shared<OBBCollider>(OBBCollider::makeCube());
        DynamicsWorld world;
        std::vector<std::shared_ptr<RigidBody>> bodies;
    };
}

// MARK: Contact manifold

/// A resting box is never perfectly flat. A tiny tilt must not remove the raised
/// corners from the manifold, otherwise the solver spins the box.
TEST(RestingContactTests, SlightlyTiltedBoxKeepsFullContactPatch) {
    OBBCollider cube = OBBCollider::makeCube();

    Transform floor;
    Transform box;
    box.position = glm::vec3(0.0f, 0.999f, 0.0f);
    box.rotation = glm::angleAxis(0.002f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.3f)));

    ContactManifold manifold = cube.testCollision(&box, &cube, &floor);

    EXPECT_EQ(manifold.contactsCount, 4u);
}

// MARK: Resting

TEST(RestingContactTests, CubeComesToRestOnStaticCube) {
    RestingWorld scene;
    auto cube = scene.addCube(0.1f, { 0.15f, 2.0f, 0.0f });

    auto motion = scene.run(cube, 8.0f, 2.0f);

    EXPECT_LT(motion.maxAngularSpeed, 1e-3f);
    EXPECT_LT(motion.heightRange, 1e-3f);
    EXPECT_LT(motion.horizontalDrift, 1e-3f);
}

TEST(RestingContactTests, CubeDroppedAtAnAngleComesToRest) {
    RestingWorld scene;
    auto cube = scene.addCube(1.0f, { 0.0f, 3.0f, 0.0f });
    cube->transform.rotation = glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));

    auto motion = scene.run(cube, 12.0f, 2.0f);

    EXPECT_LT(motion.maxAngularSpeed, 1e-2f);
    EXPECT_LT(motion.heightRange, 1e-3f);
    EXPECT_GT(cube->transform.position.y, 0.9f); // Did not tunnel through the floor.
}

TEST(RestingContactTests, CubeComesToRestOnDynamicCube) {
    RestingWorld scene;
    auto bottom = scene.addCube(10.0f, { 0.15f, 2.0f, 0.0f }, Material::Metal);
    auto top = scene.addCube(0.1f, { 0.0f, 3.5f, 0.0f });

    auto topMotion = scene.run(top, 8.0f, 2.0f);

    EXPECT_LT(topMotion.maxAngularSpeed, 1e-3f);
    EXPECT_LT(topMotion.heightRange, 1e-3f);
    EXPECT_NEAR(bottom->transform.position.y, 1.0f, 0.01f);
    EXPECT_NEAR(top->transform.position.y, 2.0f, 0.02f);
}

TEST(RestingContactTests, OffsetStackOfThreeStaysStanding) {
    RestingWorld scene;
    std::vector<std::shared_ptr<RigidBody>> cubes;
    for (int i = 0; i < 3; ++i)
    {
        cubes.push_back(scene.addCube(1.0f, { 0.15f * float(i), 1.0f * float(i + 1) + 0.02f * float(i), 0.0f }));
    }

    auto motion = scene.run(cubes.back(), 10.0f, 2.0f);

    EXPECT_LT(motion.maxAngularSpeed, 1e-2f);
    // A tall stack still creeps by a fraction of a millimetre per second as the
    // positional correction squeezes the layers; what matters is that it does not jitter or fall.
    EXPECT_LT(motion.heightRange, 1e-2f);
    EXPECT_GT(cubes.back()->transform.position.y, 2.9f);
}
