#pragma once

#include "collision/CollisionPoints.h"
#include <vector>

namespace LP 
{
    struct Solver 
    {
        virtual ~Solver() = default;

        virtual void solve(const std::vector<CollisionManifold>& collisions, float deltaTime) = 0;
    };
} // namespace LP