#pragma once

#include "collision/Solver.h"

namespace LatropPhysics
{
    struct ImpulseSolver : Solver
    {
        void solve(const std::vector<Collision>& collisions, float deltaTime) override;
    };
} // namespace LatropPhysics
