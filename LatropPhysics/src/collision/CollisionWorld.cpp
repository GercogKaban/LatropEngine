#include "collision/CollisionWorld.h"
#include "collision/Collider.h"

using namespace LatropPhysics;

void CollisionWorld::solveCollisions(
    std::vector<Collision>& collisions,
    float deltaTime
) {
    for(Solver* solver : m_solvers)
    {
        solver->solve(collisions, deltaTime);
    }
}

void CollisionWorld::sendCollisionEvents(
    std::vector<Collision>& collisions,
    float deltaTime
) {
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

    for(CollisionBody* body : m_bodies)
    {
        for(CollisionBody* other : m_bodies)
        {
            if(body == other) break;

            if(!body->collider || !other->collider) continue;

            CollisionPoints points = body->collider->testCollision(
                body->transform, other->collider, other->transform
            );

            if (points.hasCollision)
            {
                bool isTrigger = body->isTrigger || other->isTrigger;
                if(isTrigger) triggers  .push_back({ body, other, points });
                else          collisions.push_back({ body, other, points });
            }
        }
    }

    solveCollisions(collisions, deltaTime);

    sendCollisionEvents(collisions, deltaTime);
    sendCollisionEvents(triggers, deltaTime);
}