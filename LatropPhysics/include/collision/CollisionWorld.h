#pragma once

#include "Solver.h"
#include "collision/CollisionBody.h"
#include <vector>

namespace LatropPhysics 
{

struct CollisionWorld
{
public:
    inline void addCollisionBody(CollisionBody* body) {
        m_bodies.push_back(body);
    }

    inline void removeCollisionBody(CollisionBody* body) {
        auto it = std::find(m_bodies.begin(), m_bodies.end(), body);
        if (it != m_bodies.end()) {
            m_bodies.erase(it);
        }
    }

    inline void addSolver(Solver* solver) {
        m_solvers.push_back(solver);
    }

    inline void removeSolver(Solver* solver) {
        auto it = std::find(m_solvers.begin(), m_solvers.end(), solver);
        if (it != m_solvers.end()) {
            m_solvers.erase(it);
        }
    }

    void resolveCollisions(float deltaTime);
protected:
    std::vector<CollisionBody*> m_bodies;
    std::vector<Solver*> m_solvers;

    std::function<void(Collision, float)> m_onCollision;
private:
    void solveCollisions(std::vector<Collision>& collisions, float deltaTime);
    void sendCollisionEvents(std::vector<Collision>& collisions, float deltaTime); 
};

} // namespace LatropPhysics