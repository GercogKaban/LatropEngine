#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "collision/CollisionBody.h"
#include "Material.h"
#include <cassert>

namespace LP 
{
    struct RigidBody final : CollisionBody
    {
        RigidBody() : inertiaTensorLocal(glm::mat3(1.0f)), invInertiaTensorLocal(glm::mat3(1.0f)) {}

        glm::vec3 linearVelocity = glm::vec3(0.0f);
        glm::vec3 angularVelocity = glm::vec3(0.0f);

        glm::vec3 force = glm::vec3(0.0f);
        glm::vec3 torque = glm::vec3(0.0f);
        
        Material material = Material::Plastic;

        /// @brief If the rigid body takes gravity from the world.
        bool takesGravity = false;

        inline float getMass() const { return m_mass; }
        inline float getInvMass() const { return m_invMass; }

        glm::mat3 inertiaTensorLocal;
        glm::mat3 invInertiaTensorLocal;

        glm::mat3 getInvInertiaTensorWorld(const glm::quat& rotation) const
        {
            return glm::transpose(glm::mat3_cast(rotation)) * invInertiaTensorLocal * glm::mat3_cast(rotation);
        }

        virtual void setIsSimulated(bool newValue) override
        { 
            CollisionBody::setIsSimulated(newValue);
            
            if (!newValue)
            {
                m_mass = 0.0f;
                m_invMass = 0.0f;
                invInertiaTensorLocal = glm::mat3(0.0f);
                invInertiaTensorLocal = glm::mat3(0.0f);
            }
        }

        void setMass(float newValue)
        {
            assert(isSimulated() && "Setting mass for static bodies is not supported.");
            
            m_mass = newValue;
            m_invMass = newValue > 0.0f ? 1.0f / newValue : 0.0f;

            // Recompute inertia tensor for a unit cube
            float inertia = (1.0f / 12.0f) * m_mass * (1 * 1 + 1 * 1); // For a unit cube
            inertiaTensorLocal = glm::mat3(inertia);
            
            assert(inertia > 0.0f && "Inertia must be greater than zero.");
            invInertiaTensorLocal = glm::mat3(1.0f / inertia);
        }

    private:
        float m_mass = 1.0f;
        float m_invMass = 1.0f;
    };
}