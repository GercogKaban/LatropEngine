#include "collision/CollisionWorld.h"
#include "collision/Collider.h"

using namespace LatropPhysics;

void CollisionWorld::detectCollisions(std::vector<Collision>& collisions, std::vector<Collision>& triggers)
{
    // Step 1: All Static VS All Movable
    for(const std::weak_ptr<CollisionBody>& bodyWeakPtr : m_bodies)
    {
        if (bodyWeakPtr.expired())
        {
            // remove it 
            // for now continue
            continue;
        }

        auto body = bodyWeakPtr.lock().get();

        for(const std::weak_ptr<CollisionBody>& otherWeakPtr : movableBodies)
        {
            if (otherWeakPtr.expired())
            {
                // remove it 
                // for now continue
                continue;
            }

            auto other = otherWeakPtr.lock().get();

            if(body == other) break;

            if (!(body->m_isSimulated || other->m_isSimulated)) continue;

            if(body->collider.expired() || other->collider.expired()) continue;

            auto bodyCollider = body->collider.lock();
            auto otherCollider = other->collider.lock();

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
        if (bodyWeakPtr.expired())
        {
            // remove it 
            // for now continue
            continue;
        }

        auto body = bodyWeakPtr.lock().get();

        for(const std::weak_ptr<CollisionBody>& otherWeakPtr : movableBodies)
        {
            if (otherWeakPtr.expired())
            {
                // remove it 
                // for now continue
                continue;
            }

            auto other = otherWeakPtr.lock().get();

            if(body == other) break;

            if (!(body->m_isSimulated || other->m_isSimulated)) continue;

            if(body->collider.expired() || other->collider.expired()) continue;

            auto bodyCollider = body->collider.lock();
            auto otherCollider = other->collider.lock();

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