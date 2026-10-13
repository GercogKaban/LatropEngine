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

// MARK: Bouncing

/// A flat landing on a perfectly elastic surface must stay flat. Any residual spin
/// left by the solver accumulates, because nothing damps an elastic bounce.
TEST(RestingContactTests, ElasticBounceDoesNotIntroduceRotation) {
    RestingWorld scene;
    scene.bodies.front()->material.restitution = 1.0f;
    scene.bodies.front()->material.restitutionCombinator = Material::CombinationMode::Maximum;

    auto cube = scene.addCube(0.1f, { 0.0f, 2.0f, 0.0f });

    float maxAngularSpeed = 0.0f;
    float minY = 1e9f;
    for (int i = 0; i < 20000; ++i) // ~7 bounces
    {
        scene.world.integrate(fixedDelta);
        maxAngularSpeed = std::max(maxAngularSpeed, glm::length(cube->angularVelocity));
        minY = std::min(minY, cube->transform.position.y);
    }

    EXPECT_LT(maxAngularSpeed, 1e-3f);
    EXPECT_GT(minY, 0.95f); // Never sinks into the floor.
}

// MARK: Mass ratio

/// A heavy cube on a light one on the floor: the same layout as
/// `RotationalScene::createStackingScenarioI` (static, 0.1, 10, 0.1). With 20
/// iterations per step a solver that forgets its impulses every step cannot
/// carry a 100:1 load, and the tower collapses.
TEST(RestingContactTests, HeavyCubeOnLightCubeStaysStanding) {
    RestingWorld scene;
    auto lightBottom = scene.addCube(0.1f, { 0.15f, 2.0f, 0.0f });
    auto heavy = scene.addCube(10.0f, { 0.15f, 4.0f, 0.0f }, Material::Metal);
    auto lightTop = scene.addCube(0.1f, { 0.15f, 6.0f, 0.0f });

    auto motion = scene.run(lightTop, 12.0f, 2.0f);

    EXPECT_LT(motion.maxAngularSpeed, 1e-2f);
    EXPECT_LT(motion.heightRange, 1e-3f);
    EXPECT_GT(lightBottom->transform.position.y, 0.98f);
    EXPECT_NEAR(heavy->transform.position.y, 2.0f, 0.05f);
    EXPECT_NEAR(lightTop->transform.position.y, 3.0f, 0.05f);
}

/// The heavy cube must stay on top of the light one instead of squeezing through it.
TEST(RestingContactTests, HeavyCubeDroppedOnLightCubeDoesNotPassThrough) {
    RestingWorld scene;
    auto light = scene.addCube(0.1f, { 0.15f, 2.0f, 0.0f }, Material::Metal);
    auto heavy = scene.addCube(10.0f, { 0.0f, 3.5f, 0.0f });

    scene.run(heavy, 10.0f, 1.0f);

    EXPECT_GT(heavy->transform.position.y, light->transform.position.y + 0.9f);
}
