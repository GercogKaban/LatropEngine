#pragma once

#include <glm/vec3.hpp>
#include "collision/CollisionBody.h"

namespace LatropPhysics 
{
    struct RigidBody : CollisionBody
    {
        RigidBody(
            std::weak_ptr<Collider> collider,
            bool isSimulated = true, 
            bool takesGravity = false
        ) : CollisionBody(collider), 
            m_gravity(glm::vec3(0.0f)), 
            m_takesGravity(takesGravity),
            m_isSimulated(isSimulated), 
            m_staticFriction(0.0f), 
            m_dynamicFriction(0.0f), 
            m_restitution(1.0f),
            m_velocity(glm::vec3(0.0f)),
            m_force(glm::vec3(0.0f)), 
            m_mass(1.0f)
        {}

        glm::vec3 m_gravity;
        glm::vec3 m_velocity;
        glm::vec3 m_force;
        float m_mass;

        /// @brief Static friction is the friction between two objects that are not 
        /// moving relative to each other.
        float m_staticFriction;

        /// @brief Dynamic friction is the friction between two objects that are 
        /// moving relative to each other.
        float m_dynamicFriction;

        /// @brief The coefficient of restitution is a measure of how much kinetic 
        /// energy is conserved in a collision.
        /// It is the elasticity or 'bounciness' of collision.
        float m_restitution;

        /// @brief If the rigid body takes gravity from the world.
        bool m_takesGravity;

        /// @brief If the rigit body is simulated and moves, or only affects others.
        bool m_isSimulated;
    };
} 