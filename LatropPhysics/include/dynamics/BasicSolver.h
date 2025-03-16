#pragma once

#include "collision/Solver.h"

namespace LatropPhysics
{
	struct BasicSolver : Solver
	{
		void solve(std::vector<Collision>& collisions, float deltaTime) override;
	};
}