#pragma once

#include "collision/Solver.h"

namespace LP
{
    struct SmoothPositionSolver : Solver
    {
        void solve(const std::vector<CollisionManifold>& collisions, float deltaTime) override;
    };
} // namespace LP