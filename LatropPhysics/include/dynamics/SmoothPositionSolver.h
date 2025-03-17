#pragma once

#include "collision/Solver.h"

namespace LatropPhysics
{
    struct SmoothPositionSolver : Solver
    {
        void solve(const std::vector<Collision>& collisions, float deltaTime) override;
    };
} // namespace LatropPhysics