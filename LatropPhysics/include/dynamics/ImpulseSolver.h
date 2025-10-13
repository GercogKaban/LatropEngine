#pragma once

#include "collision/Solver.h"

namespace LP
{
    struct ImpulseSolver : Solver
    {
        void solve(const std::vector<CollisionManifold>& collisions, float deltaTime) override;
    };
} // namespace LP
