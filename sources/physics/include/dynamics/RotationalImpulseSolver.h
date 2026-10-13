#pragma once

#include "collision/Solver.h"
#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>

namespace LP
{
    struct RotationalImpulseSolver : Solver
    {
        void solve(const std::vector<CollisionManifold>& collisions, float deltaTime) override;

    private:
        /// The impulse a contact ended the previous step with, used to warm start
        /// the contact that continues it.
        struct CachedContact
        {
            glm::vec3 point;
            float normalImpulse;
            glm::vec3 tangentImpulse;
        };

        struct PairKey
        {
            const void* a;
            const void* b;

            bool operator==(const PairKey& other) const { return a == other.a && b == other.b; }
        };

        struct PairKeyHash
        {
            size_t operator()(const PairKey& key) const
            {
                return std::hash<const void*>()(key.a) * 31u + std::hash<const void*>()(key.b);
            }
        };

        std::unordered_map<PairKey, std::vector<CachedContact>, PairKeyHash> m_cache;
        float m_previousDeltaTime = 0.0f;
    };
} // namespace LP
