#pragma once

namespace LatropPhysics 
{

struct CollisionWorld
{
    virtual void update(float deltaTime) = 0;
};

} // namespace LatropPhysics