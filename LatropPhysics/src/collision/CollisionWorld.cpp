#include "collision/CollisionWorld.h"
#include "collision/Collider.h"

using namespace LatropPhysics;

void CollisionWorld::solveCollisions(std::vector<Collision>& collisions, float deltaTime)
{
    for(const std::unique_ptr<Solver>& solver : m_solvers)
    {
        solver.get()->solve(collisions, deltaTime);
    }
}

void CollisionWorld::sendCollisionEvents(std::vector<Collision>& collisions, float deltaTime) 
{
    for(Collision& collision : collisions)
    {
        m_onCollision(collision, deltaTime);

        auto& callbackA = collision.bodyA->m_onCollision;
        auto& callbackB = collision.bodyB->m_onCollision;

        if (callbackA) callbackA(collision, deltaTime);
        if (callbackB) callbackB(collision, deltaTime);
    }
}

void CollisionWorld::resolveCollisions(float deltaTime)
 {
    std::vector<Collision> collisions;
    std::vector<Collision> triggers;

    for(std::weak_ptr<CollisionBody> bodyWeakPtr : m_bodies)
    {
        if (bodyWeakPtr.expired())
        {
            // remove it 
            // for now continue
            continue;
        }

        auto body = bodyWeakPtr.lock().get();

        for(std::weak_ptr<CollisionBody> otherWeakPtr : m_bodies)
        {
            if (otherWeakPtr.expired())
            {
                // remove it 
                // for now continue
                continue;
            }

            auto other = otherWeakPtr.lock().get();

            if(body == other) break;

            if(body->collider.expired() || !other->collider.expired()) continue;

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

    solveCollisions(collisions, deltaTime);

    // sendCollisionEvents(collisions, deltaTime);
    // sendCollisionEvents(triggers, deltaTime);
}