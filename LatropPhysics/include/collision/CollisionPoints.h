#pragma once

#include <glm/vec3.hpp>
#include <array>

namespace LP 
{
    struct CollisionBody;

    struct ContactPoint 
    {
        /// @brief The furthest point of `start` into `end`.
        glm::vec3 start = glm::vec3(0.0f);

        /// @brief The furthest point of `end` into `start`.
        glm::vec3 end = glm::vec3(0.0f);

        /// @brief Length of `end` - `start`.
        float depth = 0.0f;
    };

    struct ContactManifold
    {
        std::array<ContactPoint, 4> contactPoints;
        glm::vec3 normal = glm::vec3(0.0f);

        uint8_t contactsCount = 0;

        inline bool hasCollision() const { return contactsCount != 0; }
    };

    struct CollisionManifold
    {
        std::array<ContactPoint, 4> contactPoints;
        glm::vec3 normal;

        CollisionBody* bodyA;
        CollisionBody* bodyB;

        uint8_t contactsCount;

        inline bool hasCollision() const { return contactsCount != 0; }

        CollisionManifold(CollisionBody* bodyA, CollisionBody* bodyB, const ContactManifold& manifold)
            : bodyA(bodyA), bodyB(bodyB), 
              contactPoints(manifold.contactPoints),
              normal(manifold.normal),
              contactsCount(manifold.contactsCount)
        {}
    };
} // namespace LP