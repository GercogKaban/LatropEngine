#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "collision/CollisionBody.h"

namespace LatropPhysics 
{
    struct RigidBody : CollisionBody
    {
        RigidBody(bool isSimulated = true, bool takesGravity = false) : m_isSimulated(isSimulated), m_takesGravity(takesGravity)
        {
            
        };

        glm::vec3 m_gravity = glm::vec3(0);

        /// @brief If the rigid body takes gravity from the world.
        bool m_takesGravity;

        /// @brief If the rigit body is simulated and moves, or only affects others.
        bool m_isSimulated;

        /// @brief Static friction is the friction between two objects that are not 
        /// moving relative to each other.
        float m_staticFriction = 0;

        /// @brief Dynamic friction is the friction between two objects that are 
        /// moving relative to each other.
        float m_dynamicFriction = 0;

        /// @brief The coefficient of restitution is a measure of how much kinetic 
        /// energy is conserved in a collision.
        /// It is the elasticity or 'bounciness' of collision.
        float m_restitution = 1;

        // private:
        glm::vec3 m_velocity = glm::vec3(0);
        glm::vec3 m_force = glm::vec3(0);
        float m_mass = 1;
    };
} 