#include "collision/CollisionWorld.h"
#include "collision/Collider.h"

using namespace LatropPhysics;

void CollisionWorld::detectCollisions(std::vector<Collision>& collisions, std::vector<Collision>& triggers)
{
    // Step 1: All Static VS All Movable
    for(const std::weak_ptr<CollisionBody>& bodyWeakPtr : m_bodies)
    {
        auto bodyLocked = bodyWeakPtr.lock();
        if (!bodyLocked) continue;
        auto body = bodyLocked.get();

        for(const std::weak_ptr<CollisionBody>& otherWeakPtr : movableBodies)
        {
            auto otherLocked = otherWeakPtr.lock();
            if (!otherLocked) continue;
            auto other = otherLocked.get();

            if(body == other) break;

            if (!(body->m_isSimulated || other->m_isSimulated)) continue;

            auto bodyCollider = body->collider.lock();
            auto otherCollider = other->collider.lock();
            if (!bodyCollider || !otherCollider) continue;

            CollisionPoints points = bodyCollider->testCollision(&body->transform, otherCollider.get(), &other->transform);

            if (points.hasCollision)
            {
                if (bool isTrigger = body->isTrigger || other->isTrigger)
                {
                    triggers.push_back({ body, other, points});
                }
                else
                {
                    collisions.push_back({ body, other, points });
                }
            }
        }
    }

    // Step 2: All Movable VS All Movable
    for(const std::weak_ptr<CollisionBody>& bodyWeakPtr : movableBodies)
    {
        auto bodyLocked = bodyWeakPtr.lock();
        if (!bodyLocked) continue;
        auto body = bodyLocked.get();

        for(const std::weak_ptr<CollisionBody>& otherWeakPtr : movableBodies)
        {
            auto otherLocked = otherWeakPtr.lock();
            if (!otherLocked) continue;
            auto other = otherLocked.get();

            if(body == other) break;

            if (!(body->m_isSimulated || other->m_isSimulated)) continue;

            auto bodyCollider = body->collider.lock();
            auto otherCollider = other->collider.lock();
            if (!bodyCollider || !otherCollider) continue;

            CollisionPoints points = bodyCollider->testCollision(&body->transform, otherCollider.get(), &other->transform);

            if (points.hasCollision)
            {
                if (bool isTrigger = body->isTrigger || other->isTrigger)
                {
                    triggers.push_back({ body, other, points});
                }
                else
                {
                    collisions.push_back({ body, other, points });
                }
            }
        }
    }
}

void CollisionWorld::solveCollisions(const std::vector<Collision>& collisions, float deltaTime)
{
    for(const std::unique_ptr<Solver>& solver : m_solvers)
    {
        solver.get()->solve(collisions, deltaTime);
    }
}

void CollisionWorld::sendCollisionEvents(const std::vector<Collision>& collisions, float deltaTime) 
{
    for(const Collision& collision : collisions)
    {
        m_onCollision(collision, deltaTime);

        collision.bodyA->m_onCollision(collision, deltaTime);
        collision.bodyB->m_onCollision(collision, deltaTime);
    }
}

void CollisionWorld::resolveCollisions(float deltaTime)
 {
    std::vector<Collision> collisions;
    std::vector<Collision> triggers;

    detectCollisions(collisions, triggers);
    solveCollisions(collisions, deltaTime);

    sendCollisionEvents(collisions, deltaTime);
    sendCollisionEvents(triggers, deltaTime);
}