#include <dynamics/DynamicsWorld.h>
#include <tracy/Tracy.hpp>

namespace LP
{
    struct TraceableDynamicsWorld final : DynamicsWorld 
    {
        virtual void detectCollisions(std::vector<CollisionManifold>& collisions, std::vector<CollisionManifold>& triggers) override
        {
            ZoneScoped;
            DynamicsWorld::detectCollisions(collisions, triggers);
        }
        
        virtual void resolveCollisions(float deltaTime) override
        {
            ZoneScoped;
            DynamicsWorld::resolveCollisions(deltaTime);
        }

        virtual void solveCollisions(const std::vector<CollisionManifold>& collisions, float deltaTime) override
        {
            ZoneScoped;
            DynamicsWorld::solveCollisions(collisions, deltaTime);
        }

        virtual void integrate(float deltaTime) override
        {
            ZoneScoped;
            DynamicsWorld::integrate(deltaTime);
        }

        virtual void applyGravity() override
        {
            ZoneScoped;
            DynamicsWorld::applyGravity();
        }

        virtual void moveBodies(float deltaTime) override
        {
            ZoneScoped;
            DynamicsWorld::moveBodies(deltaTime);
        }
    };
} // LatropPhysics