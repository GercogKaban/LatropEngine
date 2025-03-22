#pragma once

#include "collision/Solver.h"

namespace LP
{
    struct PositionSolver : Solver
    {
        void solve(const std::vector<Collision>& collisions, float deltaTime) override;
    };
} // namespace LP