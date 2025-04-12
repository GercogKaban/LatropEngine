#include "collision/CollisionWorld.h"
#include "collision/Collider.h"
#include "shared/AABB.h"

using namespace LP;

// MARK: Spacial Partitioning
int64_t computeCellKey(const glm::vec3& position, float cellSize) {
    int64_t x = static_cast<int64_t> (position.x / cellSize);
    int64_t y = static_cast<int64_t>(position.y / cellSize);
    int64_t z = static_cast<int64_t>(position.z / cellSize);

    // Improved hash using larger primes and better mixing
    const int64_t prime1 = 73856093LL;
    const int64_t prime2 = 19349663LL;
    const int64_t prime3 = 83492791LL;
    return (x * prime1) + (y * prime2) + (z * prime3);
}

void CollisionWorld::updateSpacialPartitioningOfStaticBodies(float cellSize)
{
    m_spatialHashGrid.clear();
    m_cellSize = cellSize;

    for(const std::weak_ptr<CollisionBody>& bodyWeakPtr : m_bodies)
    {
        auto bodyLocked = bodyWeakPtr.lock();
        if (!bodyLocked) continue;
        auto body = bodyLocked.get();

        auto bodyCollider = body->collider.lock();
        if (!bodyCollider) continue;

        // Get the AABB of the body
        AABB aabb = bodyCollider->getAABB(&body->transform);

        // Calculate the range of cells the AABB spans
        glm::ivec3 minCell = glm::floor(aabb.minExtents / m_cellSize);
        glm::ivec3 maxCell = glm::ceil(aabb.maxExtents / m_cellSize);

        // Iterate over all overlapping cells and insert the object
        for (int x = minCell.x; x < maxCell.x; ++x)
        {
            for (int y = minCell.y; y < maxCell.y; ++y)
            {
                for (int z = minCell.z; z < maxCell.z; ++z)
                {
                    int64_t hash = computeCellKey(glm::vec3(x, y, z) * m_cellSize, m_cellSize);
                    m_spatialHashGrid[hash].push_back(bodyWeakPtr);
                }
            }
        }
    }

    m_bodies.clear();
}

// MARK: Collision Handling
template <typename A, typename B>
inline void detectInvidualCollisionsOfAnd(
    const A& body,
    const B& other,
    std::vector<CollisionManifold>& collisions,
    std::vector<CollisionManifold>& triggers
) {
    if (!(body->isSimulated() || other->isSimulated())) return;

    auto bodyCollider = body->collider.lock();
    auto otherCollider = other->collider.lock();
    if (!bodyCollider || !otherCollider) return;

    ContactManifold points = bodyCollider->testCollision(&body->transform, otherCollider.get(), &other->transform);

    if (points.hasCollision())
    {
        if (bool isTrigger = body->isTrigger || other->isTrigger)
        {
            triggers.push_back(CollisionManifold(body, other, points));
        }
        else
        {
            collisions.push_back(CollisionManifold(body, other, points));
        }
    }
}

template <typename A, typename B>
inline void detectInvidualCollisionsOfIn(
    const B& other,
    const std::vector<A>& elements,
    std::vector<CollisionManifold>& collisions,
    std::vector<CollisionManifold>& triggers
) {
    for (const std::weak_ptr<CollisionBody> &otherWeakPtr : elements)
    {
        auto otherLocked = otherWeakPtr.lock();
        if (!otherLocked)
            continue;
        auto body = otherLocked.get();

        if (body == other)
            break;

        detectInvidualCollisionsOfAnd(body, other, collisions, triggers);
    }
}

template <typename A, typename B>
inline void detectInvidualCollisionsIn(
    const std::vector<A>& lhs,
    const std::vector<B>& rhs,
    std::vector<CollisionManifold>& collisions, 
    std::vector<CollisionManifold>& triggers
) {
    for(const std::weak_ptr<CollisionBody>& bodyWeakPtr : lhs)
    {
        auto bodyLocked = bodyWeakPtr.lock();
        if (!bodyLocked) continue;
        auto body = bodyLocked.get();

        detectInvidualCollisionsOfIn(body, rhs, collisions, triggers);
    }
}

void CollisionWorld::detectCollisions(std::vector<CollisionManifold>& collisions, std::vector<CollisionManifold>& triggers)
{
    // Step 1: All Static VS All Movable
    for(const std::weak_ptr<RigidBody>& bodyWeakPtr : movableBodies)
    {    
        m_spatialCheckBodies.clear();

        auto bodyLocked = bodyWeakPtr.lock();
        if (!bodyLocked) continue;
        auto body = bodyLocked.get();

        // Get the AABB of the body to determine overlapping cells
        auto bodyCollider = body->collider.lock();
        if (!bodyCollider) continue;
        AABB aabb = bodyCollider->getAABB(&body->transform);

        // Calculate the range of overlapping cells
        glm::ivec3 minCell = glm::floor(aabb.minExtents / m_cellSize);
        glm::ivec3 maxCell = glm::ceil(aabb.maxExtents / m_cellSize);

        for (int x = minCell.x; x < maxCell.x; ++x)
        {
            for (int y = minCell.y; y < maxCell.y; ++y)
            {
                for (int z = minCell.z; z < maxCell.z; ++z)
                {
                    int64_t hash = computeCellKey(glm::vec3(x, y, z) * m_cellSize, m_cellSize);
                    if (m_spatialHashGrid.find(hash) == m_spatialHashGrid.end()) continue;

                    auto cell = m_spatialHashGrid[hash];

                    // Detect collisions with objects in the current cell
                    for (const std::weak_ptr<CollisionBody>& otherWeakPtr : cell)
                    {
                        auto otherLocked = otherWeakPtr.lock();
                        if (!otherLocked || m_spatialCheckBodies.count(otherLocked)) continue;

                        m_spatialCheckBodies.insert(otherLocked);
                        detectInvidualCollisionsOfAnd(otherLocked.get(), body, collisions, triggers);
                    }
                }
            }
        }
    }

    // Step 2: All Movable VS All Movable
    detectInvidualCollisionsIn(movableBodies, movableBodies, collisions, triggers);
}

void CollisionWorld::solveCollisions(const std::vector<CollisionManifold>& collisions, float deltaTime)
{
    for(const std::unique_ptr<Solver>& solver : m_solvers)
    {
        solver.get()->solve(collisions, deltaTime);
    }
}

void CollisionWorld::sendCollisionEvents(const std::vector<CollisionManifold>& collisions, float deltaTime) 
{
    for(const CollisionManifold& collision : collisions)
    {
        onCollision(collision, deltaTime);

        collision.bodyA->onCollision(collision, deltaTime);
        collision.bodyB->onCollision(collision, deltaTime);
    }
}

void CollisionWorld::resolveCollisions(float deltaTime)
 {
    std::vector<CollisionManifold> collisions;
    std::vector<CollisionManifold> triggers;

    detectCollisions(collisions, triggers);
    solveCollisions(collisions, deltaTime);

    sendCollisionEvents(collisions, deltaTime);
    sendCollisionEvents(triggers, deltaTime);
}