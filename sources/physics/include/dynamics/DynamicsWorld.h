#pragma once

#include "collision/CollisionWorld.h"
#include "RigidBody.h"
#include <glm/vec3.hpp>

namespace LP 
{
    struct DynamicsWorld : CollisionWorld
    {
        glm::vec3 gravity = { 0.0f, -9.81f, 0.0f };
        float timeScale = 1.0f;

        /// @brief If bodies that came to rest are put to sleep. Sleeping bodies are not
        /// simulated until something touches them or game code moves them.
        bool allowsSleeping = true;

        /// @brief Speed, measured at the surface of the body (linear and rotational
        /// together), below which a body counts as being at rest.
        float sleepVelocityThreshold = 0.05f;

        /// @brief How long all bodies of an island have to be at rest before it sleeps.
        float timeToSleep = 0.5f;

        void addRigidBody(std::weak_ptr<RigidBody> body) 
        {
            if (body.lock()->isSimulated())
            {
                movableBodies.push_back(body);
            }
            else
            {
                addCollisionBody(body);
            }
        }

        /// @brief Wakes up every sleeping body. Needed when something that sleeping bodies
        /// rest on changed in a way the world cannot see, like a static body being removed.
        void wakeAllBodies();

        virtual void integrate(float deltaTime);
        virtual void applyGravity();
        virtual void moveBodies(float deltaTime);

        virtual void solveCollisions(const std::vector<CollisionManifold>& collisions, float deltaTime) override;
        virtual bool wakeSleepingBodies(const std::vector<CollisionManifold>& collisions) override;

    private:
        void wakeDisturbedBodies();
        void updateSleeping(const std::vector<CollisionManifold>& collisions, float deltaTime);

        uint32_t m_nextSleepGroup = 1;
        size_t m_expiredBodiesCount = 0;
        glm::vec3 m_lastGravity = gravity;
    };
} // namespace LP
