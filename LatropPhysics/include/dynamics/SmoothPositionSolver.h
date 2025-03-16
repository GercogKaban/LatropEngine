#pragma once

#include "collision/Solver.h"

namespace LatropPhysics
{

struct SmoothPositionSolver : Solver
{
    void solve(std::vector<Collision>& collisions, float deltaTime) override;
};

} // namespace LatropPhysics