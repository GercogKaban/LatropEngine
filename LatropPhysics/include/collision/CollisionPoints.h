#pragma once

#include <glm/vec3.hpp>
#include <array>

namespace LP 
{
    struct CollisionBody;

    struct ContactManifold
    {
        std::array<glm::vec3, 4> contactPoints;
        glm::vec3 normal = glm::vec3(0.0f);

        float depth = 0.0f;
        uint8_t contactsCount = 0;

        inline bool hasCollision() const { return contactsCount != 0; }
    };

    struct CollisionManifold
    {
        std::array<glm::vec3, 4> contactPoints;
        glm::vec3 normal;

        CollisionBody* bodyA;
        CollisionBody* bodyB;

        float depth = 0.0f;
        uint8_t contactsCount;

        inline bool hasCollision() const { return contactsCount != 0; }

        CollisionManifold(CollisionBody* bodyA, CollisionBody* bodyB, const ContactManifold& manifold)
            : bodyA(bodyA), bodyB(bodyB), 
              contactPoints(manifold.contactPoints),
              normal(manifold.normal),
              depth(manifold.depth),
              contactsCount(manifold.contactsCount)
        {}
    };
} // namespace LP