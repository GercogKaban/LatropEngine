#pragma once

#include <glm/vec3.hpp>
#include "collision/CollisionBody.h"
#include "Material.h"
#include <cassert>

namespace LP 
{
    struct RigidBody final : CollisionBody
    {
        RigidBody(){}

        glm::vec3 linearVelocity = glm::vec3(0.0f);
        glm::vec3 angularVelocity = glm::vec3(0.0f);

        glm::vec3 force = glm::vec3(0.0f);
        glm::vec3 torque = glm::vec3(0.0f);
        
        Material material = Material::Plastic;

        /// @brief If the rigid body takes gravity from the world.
        bool takesGravity = false;

        inline float getMass() const { return m_mass; }
        inline float getInvMass() const { return m_invMass; }

        virtual void setIsSimulated(bool newValue) override
        { 
            CollisionBody::setIsSimulated(newValue);
            
            if (!newValue)
            {
                m_mass = 0.0f;
                m_invMass = 0.0f;
            }
        }

        void setMass(float newValue)
        {
            assert(isSimulated() && "Setting mass for static bodies is not supported.");
            
            m_mass = newValue;
            m_invMass = newValue > 0.0f ? 1.0f / newValue : 0.0f;
        }

    private:
        float m_mass = 1.0f;
        float m_invMass = 1.0f;
    };
} 