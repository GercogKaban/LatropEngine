#include <gtest/gtest.h>
#include "RestingWorld.h"

using namespace LP;

namespace
{
    void step(RestingWorld& scene, float seconds)
    {
        for (int i = 0; i < int(seconds / fixedDelta); ++i) scene.world.integrate(fixedDelta);
    }

    bool allAsleep(const std::vector<std::shared_ptr<RigidBody>>& cubes)
    {
        return std::all_of(cubes.begin(), cubes.end(), [](const auto& cube) { return cube->isAsleep(); });
    }

    /// A cube that fell onto the floor and has been left alone until it slept.
    std::shared_ptr<RigidBody> addSleepingCube(RestingWorld& scene, glm::vec3 position)
    {
        auto cube = scene.addCube(1.0f, position);
        step(scene, 4.0f);
        return cube;
    }
}

// MARK: Falling asleep

TEST(SleepingTests, CubeAtRestFallsAsleep) {
    RestingWorld scene;
    auto cube = scene.addCube(1.0f, { 0.0f, 2.0f, 0.0f });

    step(scene, 4.0f);

    EXPECT_TRUE(cube->isAsleep());
    EXPECT_EQ(cube->linearVelocity, glm::vec3(0.0f));
    EXPECT_EQ(cube->angularVelocity, glm::vec3(0.0f));
}

TEST(SleepingTests, SleepingCubeIsExactlyStill) {
    RestingWorld scene;
    auto cube = addSleepingCube(scene, { 0.15f, 2.0f, 0.0f });
    ASSERT_TRUE(cube->isAsleep());

    glm::vec3 position = cube->transform.position;
    glm::quat rotation = cube->transform.rotation;

    step(scene, 2.0f);

    EXPECT_TRUE(cube->isAsleep());
    EXPECT_EQ(cube->transform.position, position);
    EXPECT_EQ(cube->transform.rotation, rotation);
}

TEST(SleepingTests, CubeDoesNotSleepWhileFalling) {
    RestingWorld scene;
    auto cube = scene.addCube(1.0f, { 0.0f, 30.0f, 0.0f });

    step(scene, 1.5f);

    EXPECT_FALSE(cube->isAsleep());
}

TEST(SleepingTests, CubeThatOptsOutNeverSleeps) {
    RestingWorld scene;
    auto cube = scene.addCube(1.0f, { 0.0f, 2.0f, 0.0f });
    cube->allowsSleeping = false;

    step(scene, 4.0f);

    EXPECT_FALSE(cube->isAsleep());
}

TEST(SleepingTests, SleepingCanBeDisabledForTheWorld) {
    RestingWorld scene;
    scene.world.allowsSleeping = false;
    auto cube = scene.addCube(1.0f, { 0.0f, 2.0f, 0.0f });

    step(scene, 4.0f);

    EXPECT_FALSE(cube->isAsleep());
}

TEST(SleepingTests, SeparateIslandsSleepIndependently) {
    RestingWorld scene;
    auto resting = scene.addCube(1.0f, { -2.0f, 2.0f, 0.0f });
    auto falling = scene.addCube(1.0f, { 2.0f, 60.0f, 0.0f });

    step(scene, 4.0f);

    EXPECT_TRUE(resting->isAsleep());
    EXPECT_FALSE(falling->isAsleep());
}

// MARK: Islands

TEST(SleepingTests, StackSleepsTogether) {
    RestingWorld scene;
    std::vector<std::shared_ptr<RigidBody>> cubes;
    for (int i = 0; i < 4; ++i)
    {
        cubes.push_back(scene.addCube(1.0f, { 0.0f, 1.5f * float(i + 1), 0.0f }));
    }

    step(scene, 10.0f);

    EXPECT_TRUE(allAsleep(cubes));
    for (const auto& cube : cubes) EXPECT_EQ(cube->getSleepGroup(), cubes.front()->getSleepGroup());
}

/// The 100:1 tower from `RotationalScene::createStackingScenarioI`.
TEST(SleepingTests, HeavyAndLightTowerComesToACompleteStop) {
    RestingWorld scene;
    std::vector<std::shared_ptr<RigidBody>> cubes = {
        scene.addCube(0.1f, { 0.15f, 2.0f, 0.0f }),
        scene.addCube(10.0f, { 0.15f, 4.0f, 0.0f }, Material::Metal),
        scene.addCube(0.1f, { 0.15f, 6.0f, 0.0f })
    };

    step(scene, 15.0f);

    EXPECT_TRUE(allAsleep(cubes));
}

// MARK: Waking up

TEST(SleepingTests, DroppedCubeWakesSleepingStackAndDoesNotPassThrough) {
    RestingWorld scene;
    auto bottom = addSleepingCube(scene, { 0.0f, 2.0f, 0.0f });
    auto top = scene.addCube(1.0f, { 0.0f, 3.0f, 0.0f });
    step(scene, 4.0f);
    ASSERT_TRUE(bottom->isAsleep());
    ASSERT_TRUE(top->isAsleep());

    auto dropped = scene.addCube(1.0f, { 0.0f, 8.0f, 0.0f });
    bool bottomWoke = false;
    for (int i = 0; i < int(2.0f / fixedDelta); ++i)
    {
        scene.world.integrate(fixedDelta);
        bottomWoke = bottomWoke || !bottom->isAsleep();
    }

    EXPECT_TRUE(bottomWoke) << "The impact has to reach the bottom of the stack";
    EXPECT_NEAR(bottom->transform.position.y, 1.0f, 0.05f);
    EXPECT_NEAR(top->transform.position.y, 2.0f, 0.05f);
    EXPECT_NEAR(dropped->transform.position.y, 3.0f, 0.05f);

    step(scene, 6.0f);
    EXPECT_TRUE(allAsleep({ bottom, top, dropped }));
}

/// Waking by contact has to happen before the collisions are solved. Otherwise the
/// solver pushes a body that is meant to be frozen, and the bodies it rests on are
/// not part of the collisions at all, as nothing tested them while they slept.
TEST(SleepingTests, ContactWithAwakeBodyWakesTheWholeGroupBeforeSolving) {
    RestingWorld scene;
    auto bottom = scene.addCube(1.0f, { 0.0f, 2.0f, 0.0f });
    auto top = scene.addCube(1.0f, { 0.0f, 3.0f, 0.0f });
    step(scene, 5.0f);
    ASSERT_TRUE(bottom->isAsleep() && top->isAsleep());

    auto visitor = scene.addCube(1.0f, { 0.0f, 2.9f, 0.0f }); // Overlaps the top face of the stack (at y = 2.5).

    std::vector<CollisionManifold> collisions, triggers;
    scene.world.detectCollisions(collisions, triggers);
    ASSERT_FALSE(collisions.empty());
    EXPECT_TRUE(scene.world.wakeSleepingBodies(collisions));

    EXPECT_FALSE(top->isAsleep());
    EXPECT_FALSE(bottom->isAsleep());
}

TEST(SleepingTests, GivingSleepingCubeVelocityWakesIt) {
    RestingWorld scene;
    auto cube = addSleepingCube(scene, { 0.0f, 2.0f, 0.0f });
    ASSERT_TRUE(cube->isAsleep());

    cube->linearVelocity = { 1.0f, 0.0f, 0.0f };
    scene.world.integrate(fixedDelta);

    EXPECT_FALSE(cube->isAsleep());
    EXPECT_GT(cube->transform.position.x, 0.0f);
}

TEST(SleepingTests, ApplyingForceToSleepingCubeWakesIt) {
    RestingWorld scene;
    auto cube = addSleepingCube(scene, { 0.0f, 2.0f, 0.0f });
    ASSERT_TRUE(cube->isAsleep());

    cube->force = { 50.0f, 0.0f, 0.0f };
    scene.world.integrate(fixedDelta);

    EXPECT_FALSE(cube->isAsleep());
}

TEST(SleepingTests, MovingSleepingCubeWakesIt) {
    RestingWorld scene;
    auto cube = addSleepingCube(scene, { 0.0f, 2.0f, 0.0f });
    ASSERT_TRUE(cube->isAsleep());

    cube->transform.position = { 2.0f, 6.0f, 0.0f }; // Like a portal teleport.
    scene.world.integrate(fixedDelta);

    EXPECT_FALSE(cube->isAsleep());

    step(scene, 3.0f);
    EXPECT_LT(cube->transform.position.y, 5.0f) << "It has to fall, not hang in the air";
}

TEST(SleepingTests, WakingOneCubeWakesTheRestOfItsStack) {
    RestingWorld scene;
    auto bottom = scene.addCube(1.0f, { 0.0f, 2.0f, 0.0f });
    auto top = scene.addCube(1.0f, { 0.0f, 3.0f, 0.0f });
    step(scene, 5.0f);
    ASSERT_TRUE(bottom->isAsleep() && top->isAsleep());

    bottom->linearVelocity = { 0.5f, 0.0f, 0.0f };
    scene.world.integrate(fixedDelta);

    EXPECT_FALSE(bottom->isAsleep());
    EXPECT_FALSE(top->isAsleep());
}

TEST(SleepingTests, RemovingTheSupportWakesTheStack) {
    RestingWorld scene;
    auto bottom = scene.addCube(1.0f, { 0.0f, 2.0f, 0.0f });
    auto top = scene.addCube(1.0f, { 0.0f, 3.0f, 0.0f });
    step(scene, 5.0f);
    ASSERT_TRUE(bottom->isAsleep() && top->isAsleep());

    scene.bodies.erase(std::remove(scene.bodies.begin(), scene.bodies.end(), bottom), scene.bodies.end());
    bottom.reset();

    scene.world.integrate(fixedDelta);
    EXPECT_FALSE(top->isAsleep());

    step(scene, 2.0f);
    EXPECT_NEAR(top->transform.position.y, 1.0f, 0.05f) << "It has to fall to the floor";
}

TEST(SleepingTests, ChangingGravityWakesSleepingBodies) {
    RestingWorld scene;
    auto cube = addSleepingCube(scene, { 0.0f, 2.0f, 0.0f });
    ASSERT_TRUE(cube->isAsleep());

    scene.world.gravity = { 0.0f, 9.81f, 0.0f };
    scene.world.integrate(fixedDelta);

    EXPECT_FALSE(cube->isAsleep());
}

TEST(SleepingTests, WakeAllBodiesWakesEverything) {
    RestingWorld scene;
    auto cube = addSleepingCube(scene, { 0.0f, 2.0f, 0.0f });
    ASSERT_TRUE(cube->isAsleep());

    scene.world.wakeAllBodies();

    EXPECT_FALSE(cube->isAsleep());
}

// MARK: Triggers

/// A sleeping body inside a trigger is still inside it, so the trigger keeps reporting it.
TEST(SleepingTests, SleepingCubeInsideTriggerStillReportsIt) {
    RestingWorld scene;
    auto cube = addSleepingCube(scene, { 0.0f, 2.0f, 0.0f });

    auto trigger = scene.addCube(1.0f, { 0.0f, 1.0f, 0.0f });
    trigger->takesGravity = false;
    trigger->isTrigger = true;
    trigger->allowsSleeping = true;
    int reports = 0;
    trigger->onCollision = [&reports](const CollisionManifold&, float) { ++reports; };

    // The trigger sleeps as well, which must not silence it.
    step(scene, 3.0f);
    ASSERT_TRUE(cube->isAsleep());
    ASSERT_TRUE(trigger->isAsleep());

    reports = 0;
    step(scene, 1.0f);

    EXPECT_GT(reports, 900);
}
