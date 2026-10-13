#pragma once

#include <glm/vec3.hpp>
#include <array>

namespace LP 
{
    struct CollisionBody;

    struct ContactManifold
    {
        constexpr static uint32_t maxContactsCountIn3D = 4;
        std::array<glm::vec3, maxContactsCountIn3D> contactPoints;
        /// @brief Per-contact penetration. Negative means the point is still
        /// separated from the other body by that distance.
        std::array<float, maxContactsCountIn3D> penetrations = {};
        glm::vec3 normal = glm::vec3(0.0f);

        /// @brief The deepest penetration among the contacts.
        float depth = 0.0f;
        uint32_t contactsCount = 0;

        inline bool hasCollision() const { return contactsCount != 0; }
    };

    struct CollisionManifold
    {
        std::array<glm::vec3, ContactManifold::maxContactsCountIn3D> contactPoints;
        std::array<float, ContactManifold::maxContactsCountIn3D> penetrations;
        glm::vec3 normal;

        CollisionBody* bodyA;
        CollisionBody* bodyB;

        float depth = 0.0f;
        uint32_t contactsCount;

        inline bool hasCollision() const { return contactsCount != 0; }

        CollisionManifold(CollisionBody* bodyA, CollisionBody* bodyB, const ContactManifold& manifold)
            : bodyA(bodyA), bodyB(bodyB), 
              contactPoints(manifold.contactPoints),
              penetrations(manifold.penetrations),
              normal(manifold.normal),
              depth(manifold.depth),
              contactsCount(manifold.contactsCount)
        {}
    };
} // namespace LP