#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "collision/CollisionBody.h"
#include "Material.h"
#include <cassert>
#include <cstdint>

namespace LP 
{
    struct RigidBody final : CollisionBody
    {
        RigidBody() {}

        glm::vec3 linearVelocity = glm::vec3(0.0f);
        glm::vec3 angularVelocity = glm::vec3(0.0f);

        glm::vec3 force = glm::vec3(0.0f);
        glm::vec3 torque = glm::vec3(0.0f);
        
        Material material = Material::Plastic;

        /// @brief If the rigid body takes gravity from the world.
        bool takesGravity = false;

        bool freezesRotation = false;

        /// @brief If the body is allowed to fall asleep once it comes to rest. Bodies that
        /// are driven by game code every frame can opt out.
        bool allowsSleeping = true;

        virtual bool isAsleep() const override { return m_sleepGroup != 0; }

        /// @brief Bodies that fell asleep together share a group, and wake up together.
        inline uint32_t getSleepGroup() const { return m_sleepGroup; }

        /// @brief How long the body has been moving slower than the sleep threshold.
        inline float getQuietTime() const { return m_quietTime; }
        inline void addQuietTime(float deltaTime) { m_quietTime += deltaTime; }
        inline void resetQuietTime() { m_quietTime = 0.0f; }

        /// @brief Freezes the body in its current pose. Sleeping bodies are skipped by
        /// the simulation and act as immovable obstacles.
        void sleep(uint32_t group)
        {
            m_sleepGroup = group;
            m_sleepPosition = transform.position;
            m_sleepRotation = transform.rotation;

            linearVelocity = glm::vec3(0.0f);
            angularVelocity = glm::vec3(0.0f);
            force = glm::vec3(0.0f);
            torque = glm::vec3(0.0f);
        }

        void wake()
        {
            m_sleepGroup = 0;
            m_quietTime = 0.0f;
        }

        /// @brief Whether game code touched a sleeping body: moved it, gave it a velocity
        /// or applied a force. Such a body has to wake up, as nothing else would tell it.
        bool isDisturbedWhileAsleep() const
        {
            return isAsleep() && (
                linearVelocity != glm::vec3(0.0f) || angularVelocity != glm::vec3(0.0f) ||
                force != glm::vec3(0.0f) || torque != glm::vec3(0.0f) ||
                transform.position != m_sleepPosition || transform.rotation != m_sleepRotation
            );
        }

        inline float getMass() const { return m_mass; }
        inline float getInvMass() const { return m_invMass; }
        inline glm::mat3 getInvInertiaTensor() const 
        { 
            if (freezesRotation)
            {
                return glm::mat3(0.0f);
            }
            else 
            {
                const glm::mat3 rotation = glm::mat3_cast(transform.rotation);
                return rotation * m_invInertiaTensorLocal * glm::transpose(rotation);
            }
        }

        virtual void setIsSimulated(bool newValue) override
        { 
            CollisionBody::setIsSimulated(newValue);
            
            if (!newValue)
            {
                m_invMass = 0.0f;
                m_invInertiaTensorLocal = glm::mat3(0.0f);
            }
        }

        void setMass(float newValue)
        {
            assert(isSimulated() && "Setting mass for static bodies is not supported.");
            
            m_mass = newValue;
            m_invMass = newValue > 0.0f ? 1.0f / newValue : 0.0f;

            // Recompute the inverse of the inertia tensor.

            float w = transform.scale.x;
            float h = transform.scale.y;
            float d = transform.scale.z;
            
            float factor = m_mass / 12.0f;

            assert((w * h * d) > 0.0f && "Dimensions must be greater than zero.");
            assert(factor > 0.0f && "Inertia factor must be greater than zero.");

            float Ixx = factor * (h * h + d * d);
            float Iyy = factor * (w * w + d * d);
            float Izz = factor * (w * w + h * h);
        
            m_invInertiaTensorLocal = glm::mat3(
                1 / Ixx, 0.0f, 0.0f,
                0.0f, 1 / Iyy, 0.0f,
                0.0f, 0.0f, 1 / Izz
            );
        }

    private:
        float m_mass = 1.0f;
        float m_invMass = 1.0f;

        glm::mat3 m_invInertiaTensorLocal = glm::mat3(1.0f);

        uint32_t m_sleepGroup = 0;
        float m_quietTime = 0.0f;
        glm::vec3 m_sleepPosition = glm::vec3(0.0f);
        glm::quat m_sleepRotation = glm::quat(1.0f, glm::vec3(0.0f));
    };
}