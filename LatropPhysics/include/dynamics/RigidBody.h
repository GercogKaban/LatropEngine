#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "collision/CollisionBody.h"

namespace LatropPhysics 
{

struct RigidBody: CollisionBody
{   
public:
    glm::vec3 m_gravity;
    bool m_takesGravity;

    /// @brief Static friction is the friction between two objects that are not 
    /// moving relative to each other.
    // float m_staticFriction;

    /// @brief Dynamic friction is the friction between two objects that are 
    /// moving relative to each other.
    // float m_dynamicFriction;

    /// @brief The coefficient of restitution is a measure of how much kinetic 
    /// energy is conserved in a collision.
    /// It is the elasticity or 'bounciness' of collision.
    // float m_restitution;

// private:
    glm::vec3 m_velocity;
    glm::vec3 m_force;
    float m_mass;
};

} // namespace LatropPhysics