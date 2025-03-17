#pragma once

#include "Solver.h"
#include "collision/CollisionBody.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace LatropPhysics 
{
    struct CollisionWorld
    {
        CollisionWorld(std::function<void(const Collision&, float)> onCollision = [](auto a, auto b) {}) 
            : m_onCollision(onCollision)  
            {}

        void addCollisionBody(std::weak_ptr<CollisionBody> body) 
        {
            m_bodies.emplace_back(body);
        }

        void removeCollisionBody(std::weak_ptr<CollisionBody> body)
        {
            if (auto sharedBody = body.lock()) // Convert to shared_ptr
            {
                auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
                    [&sharedBody](const std::weak_ptr<CollisionBody>& b)
                    {
                        return !b.expired() && b.lock() == sharedBody;
                    });

                if (it != m_bodies.end())
                {
                    m_bodies.erase(it);
                }
            }
        }

        void addSolver(std::unique_ptr<Solver>&& solver) 
        {
            m_solvers.emplace_back(std::move(solver));
        }

        void removeSolver(const std::unique_ptr<Solver>& solver)
        {
            auto it = std::find_if(m_solvers.begin(), m_solvers.end(),
                [&solver](const std::unique_ptr<Solver>& s)
                {
                    return s.get() == solver.get();
                });

            if (it != m_solvers.end())
            {
                m_solvers.erase(it);
            }
        }

        void resolveCollisions(float deltaTime);

    protected:

        std::vector<std::weak_ptr<CollisionBody>> m_bodies;
        std::vector<std::unique_ptr<Solver>> m_solvers;

        std::function<void(const Collision&, float)> m_onCollision;

    private:
        void solveCollisions(const std::vector<Collision>& collisions, float deltaTime);
        void sendCollisionEvents(const std::vector<Collision>& collisions, float deltaTime); 
    };
} // namespace LatropPhysics