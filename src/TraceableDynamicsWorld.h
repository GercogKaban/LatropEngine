#include <dynamics/DynamicsWorld.h>
#include <tracy/Tracy.hpp>

namespace LatropPhysics
{
    struct TraceableDynamicsWorld : DynamicsWorld 
    {
        virtual void resolveCollisions(float deltaTime)
        {
            ZoneScoped;
            DynamicsWorld::resolveCollisions(deltaTime);
        }

        virtual void integrate(float deltaTime)
        {
            ZoneScoped;
            DynamicsWorld::integrate(deltaTime);
        }

        virtual void applyGravity()
        {
            ZoneScoped;
            DynamicsWorld::applyGravity();
        }

        virtual void moveBodies(float deltaTime)
        {
            ZoneScoped;
            DynamicsWorld::moveBodies(deltaTime);
        }
    };
} // LatropPhysics