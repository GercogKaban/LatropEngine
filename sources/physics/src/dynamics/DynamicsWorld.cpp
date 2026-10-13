#include "dynamics/DynamicsWorld.h"

#include <algorithm>
#include <limits>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

using namespace LP;

void DynamicsWorld::applyGravity()
{
    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        if (std::shared_ptr<RigidBody> rigidBody = body.lock())
        {
            if (rigidBody->isSimulated() && rigidBody->takesGravity && !rigidBody->isAsleep())
            {
                rigidBody->force = gravity * rigidBody->getMass();
            }
            else
            {
                // TODO: Remove the expired body?
            }
        }
    }
}

void DynamicsWorld::moveBodies(float deltaTime)
{
    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        if (std::shared_ptr<RigidBody> rigidBody = body.lock())
        {
            if (rigidBody->isAsleep()) continue;

            glm::vec3 oldVelocity = rigidBody->linearVelocity;
            glm::vec3 oldPosition = rigidBody->transform.position;

            // Update linear velocity
            rigidBody->linearVelocity = oldVelocity + rigidBody->force * rigidBody->getInvMass() * deltaTime;
            rigidBody->transform.position = oldPosition + oldVelocity * deltaTime;

            // Update angular velocity based on torque and inertia
            glm::vec3 angularAcceleration = rigidBody->torque * rigidBody->getInvInertiaTensor();
            rigidBody->angularVelocity += angularAcceleration * deltaTime;

            // Update rotation based on angular velocity
            glm::quat rotationDelta = glm::quat(0, rigidBody->angularVelocity.x, rigidBody->angularVelocity.y, rigidBody->angularVelocity.z) * deltaTime;
            glm::vec3 axis = glm::normalize(rigidBody->angularVelocity);
            float angle = glm::length(rigidBody->angularVelocity) * deltaTime;

            if (angle > 0.0f) {
                glm::quat deltaRotation = glm::angleAxis(angle, axis);
                rigidBody->transform.rotation = glm::normalize(deltaRotation * rigidBody->transform.rotation);
            }

            rigidBody->force = glm::vec3(0.0f);
            rigidBody->torque = glm::vec3(0.0f);  // Reset torque after applying it
        }
    }
}

void DynamicsWorld::integrate(float deltaTime)
{
    wakeDisturbedBodies();
    applyGravity();
    resolveCollisions(deltaTime * timeScale);
    moveBodies(deltaTime * timeScale);
}

// MARK: - Sleeping

void DynamicsWorld::wakeAllBodies()
{
    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        if (std::shared_ptr<RigidBody> rigidBody = body.lock()) rigidBody->wake();
    }
}

void DynamicsWorld::wakeDisturbedBodies()
{
    // Nothing tells a sleeping body that what it rests on is gone or that gravity
    // changed, so these wake everything. Both are rare.
    size_t expired = std::count_if(movableBodies.begin(), movableBodies.end(),
        [](const std::weak_ptr<RigidBody>& body) { return body.expired(); });

    bool environmentChanged = expired > m_expiredBodiesCount || gravity != m_lastGravity;
    m_expiredBodiesCount = expired;
    m_lastGravity = gravity;

    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        std::shared_ptr<RigidBody> rigidBody = body.lock();
        if (!rigidBody || !rigidBody->isAsleep()) continue;

        // Game code writes velocities, forces and positions of the bodies directly
        // (the player, portals), so the world has to notice that on its own.
        if (environmentChanged || rigidBody->isDisturbedWhileAsleep())
        {
            // The rest of its group depends on this body to stay where it is.
            uint32_t group = rigidBody->getSleepGroup();
            for (const std::weak_ptr<RigidBody>& other : movableBodies)
            {
                std::shared_ptr<RigidBody> sibling = other.lock();
                if (sibling && sibling->getSleepGroup() == group) sibling->wake();
            }
        }
    }
}

bool DynamicsWorld::wakeSleepingBodies(const std::vector<CollisionManifold>& collisions)
{
    std::unordered_set<uint32_t> groupsToWake;

    for (const CollisionManifold& manifold : collisions)
    {
        CollisionBody* sleeper = nullptr;
        CollisionBody* visitor = nullptr;

        if (manifold.bodyA->isAsleep() && !manifold.bodyB->isAsleep()) { sleeper = manifold.bodyA; visitor = manifold.bodyB; }
        if (manifold.bodyB->isAsleep() && !manifold.bodyA->isAsleep()) { sleeper = manifold.bodyB; visitor = manifold.bodyA; }

        if (!sleeper || !visitor->isSimulated()) continue;

        if (RigidBody* sleepingBody = dynamic_cast<RigidBody*>(sleeper))
        {
            groupsToWake.insert(sleepingBody->getSleepGroup());
        }
    }

    if (groupsToWake.empty()) return false;

    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        std::shared_ptr<RigidBody> rigidBody = body.lock();
        if (rigidBody && groupsToWake.count(rigidBody->getSleepGroup())) rigidBody->wake();
    }

    return true;
}

void DynamicsWorld::solveCollisions(const std::vector<CollisionManifold>& collisions, float deltaTime)
{
    CollisionWorld::solveCollisions(collisions, deltaTime);
    updateSleeping(collisions, deltaTime);
}

void DynamicsWorld::updateSleeping(const std::vector<CollisionManifold>& collisions, float deltaTime)
{
    if (!allowsSleeping || deltaTime <= 0.0f) return;

    std::vector<std::shared_ptr<RigidBody>> bodies;
    std::unordered_map<const CollisionBody*, size_t> indexOfBody;

    for (const std::weak_ptr<RigidBody>& body : movableBodies)
    {
        std::shared_ptr<RigidBody> rigidBody = body.lock();
        if (!rigidBody || !rigidBody->isSimulated() || rigidBody->isAsleep()) continue;

        indexOfBody[rigidBody.get()] = bodies.size();
        bodies.push_back(std::move(rigidBody));
    }

    if (bodies.empty()) return;

    // MARK: Rest
    // Measured after the solver and before gravity is added for the step. Afterwards
    // a body at rest always has g * dt left, which is not motion.

    const float thresholdSquared = sleepVelocityThreshold * sleepVelocityThreshold;

    for (const std::shared_ptr<RigidBody>& body : bodies)
    {
        float radius = 0.5f * glm::length(body->transform.scale);
        float motion = glm::dot(body->linearVelocity, body->linearVelocity)
                     + glm::dot(body->angularVelocity, body->angularVelocity) * radius * radius;

        if (body->allowsSleeping && motion < thresholdSquared) body->addQuietTime(deltaTime);
        else                                                   body->resetQuietTime();
    }

    // MARK: Islands
    // Bodies that touch each other sleep and wake together. Contacts with bodies
    // that cannot move do not connect islands, or the floor would join everything.

    std::vector<size_t> parent(bodies.size());
    std::iota(parent.begin(), parent.end(), size_t(0));

    auto find = [&](size_t i)
    {
        while (parent[i] != i) { parent[i] = parent[parent[i]]; i = parent[i]; }
        return i;
    };

    auto indexOf = [&](const CollisionBody* body) -> long
    {
        auto found = indexOfBody.find(body);
        return found == indexOfBody.end() ? -1 : long(found->second);
    };

    for (const CollisionManifold& manifold : collisions)
    {
        long a = indexOf(manifold.bodyA);
        long b = indexOf(manifold.bodyB);

        if (a >= 0 && b >= 0) parent[find(size_t(a))] = find(size_t(b));
    }

    struct Island
    {
        float quietTime = std::numeric_limits<float>::max();
        bool isSupported = false;
        bool takesGravity = false;
    };

    std::unordered_map<size_t, Island> islands;

    for (size_t i = 0; i < bodies.size(); ++i)
    {
        Island& island = islands[find(i)];
        island.quietTime = std::min(island.quietTime, bodies[i]->getQuietTime());
        island.takesGravity = island.takesGravity || bodies[i]->takesGravity;
    }

    for (const CollisionManifold& manifold : collisions)
    {
        long a = indexOf(manifold.bodyA);
        long b = indexOf(manifold.bodyB);

        // A contact with something that does not move is what holds the island up.
        if (a >= 0 && b < 0) islands[find(size_t(a))].isSupported = true;
        if (b >= 0 && a < 0) islands[find(size_t(b))].isSupported = true;
    }

    // MARK: Sleep

    bool anySlept = false;

    for (size_t i = 0; i < bodies.size(); ++i)
    {
        const Island& island = islands[find(i)];

        // A body in the air is momentarily at rest at the top of its jump.
        bool isSettled = island.isSupported || !island.takesGravity;

        if (isSettled && island.quietTime >= timeToSleep)
        {
            // All members of an island get the same group, picked from the root.
            bodies[i]->sleep(m_nextSleepGroup + uint32_t(find(i)));
            anySlept = true;
        }
    }

    if (anySlept) m_nextSleepGroup += uint32_t(bodies.size());
}
