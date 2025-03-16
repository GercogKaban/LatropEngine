#pragma once

#include "collision/Solver.h"

namespace LatropPhysics
{

struct PositionSolver : Solver
{
    void solve(std::vector<Collision>& collisions, float deltaTime) override;
};

} // namespace LatropPhysics