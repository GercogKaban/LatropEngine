#pragma once

#include <glm/vec3.hpp>
#include "collision/CollisionBody.h"
#include "Material.h"

namespace LP 
{
    struct RigidBody : CollisionBody
    {
        RigidBody(){}

        glm::vec3 m_velocity = glm::vec3(0.0f);
        glm::vec3 m_force = glm::vec3(0.0f);

        float m_mass = 1.0f;
        
        Material material = Material::Plastic;

        /// @brief If the rigid body takes gravity from the world.
        bool m_takesGravity = false;
    };
} 