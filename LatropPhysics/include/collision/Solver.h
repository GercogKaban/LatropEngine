#pragma once

#include "collision/CollisionPoints.h"
#include <vector>

namespace LatropPhysics 
{

struct Solver {
public:
    virtual ~Solver() = default;

    virtual void solve(std::vector<Collision>& collisions, float deltaTime) = 0;
};

} // namespace LatropPhysics