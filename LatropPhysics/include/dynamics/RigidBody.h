#pragma once

#include <glm/vec3.hpp>
#include "collision/CollisionBody.h"

namespace LatropPhysics 
{
    struct RigidBody : CollisionBody
    {
        RigidBody(){}

        glm::vec3 m_gravity = glm::vec3(0.0f);
        glm::vec3 m_velocity = glm::vec3(0.0f);
        glm::vec3 m_force = glm::vec3(0.0f);
        float m_mass = 1.0f;

        /// @brief Static friction is the friction between two objects that are not 
        /// moving relative to each other.
        float m_staticFriction = 0.0f;

        /// @brief Dynamic friction is the friction between two objects that are 
        /// moving relative to each other.
        float m_dynamicFriction = 0.0f;

        /// @brief The coefficient of restitution is a measure of how much kinetic 
        /// energy is conserved in a collision.
        /// It is the elasticity or 'bounciness' of collision.
        float m_restitution = 1.0f;

        /// @brief If the rigid body takes gravity from the world.
        bool m_takesGravity = false;
    };
} 