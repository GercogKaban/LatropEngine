#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <memory>
#include <vector>
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

