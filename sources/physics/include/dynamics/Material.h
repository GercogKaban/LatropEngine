#pragma once

namespace LP
{
    struct Material
    {
        /// @brief When two bodies are in contact, the same bounciness and friction 
        /// effect is applied to both of them according to the chosen mode. There is 
        /// a special case when the two colliders in contact have different combine 
        /// modes set. In this particular case, the function that has the highest 
        /// priority is used. The priority order is as follows:
        /// Average < Minimum < Multiply < Maximum. For example, if one material 
        /// has Average set but the other one has Maximum, then the combine function 
        /// to be used is Maximum, since it has higher priority.
        enum class CombinationMode
        {
            Average, Minimum, Multiply, Maximum
        };

        /// @brief Static friction is the friction between two objects that are not 
        /// moving relative to each other. It affects how much force must be applied
        /// for the object to start moving while standing on this material.
        float staticFriction;

        /// @brief Dynamic friction is the friction between two objects that are 
        /// moving relative to each other. It affects how much force must be applied
        /// for the object to continue movign with the same speed while already
        /// sliding on this material.
        float dynamicFriction;

        /// @brief The coefficient of restitution is a measure of how much kinetic 
        /// energy is conserved in a collision.
        /// It is the elasticity or 'bounciness' of collision. It is not directly
        /// related to friction, rather it affects to what effect friction values
        /// are applied. For example, with low restituion values object won't bounce
        /// off of this material and will experience more friction due to the need
        /// to slide sooner than if it was to bounce of into the air.
        float restitution;

        CombinationMode frictionCombinator = CombinationMode::Average;
        CombinationMode restitutionCombinator = CombinationMode::Average;

        Material combinedWith(const Material& otherMaterial) const;

        // MARK: Common surfaces

        /// @brief Slippery and low bounce
        static const Material Ice;
        /// @brief Moderate friction and low bounce
        static const Material Wood;
        /// @brief High friction and bounciness
        static const Material Rubber;
        /// @brief Low friction and medium bounce
        static const Material Metal;
        /// @brief High friction and very low bounce
        static const Material Concrete;
        /// @brief Smooth and a bit bouncy
        static const Material Glass;
        /// @brief Very high friction, low bounce
        static const Material Carpet;
        /// @brief Medium friction and bounce
        static const Material Plastic;

        // MARK: Specialized materials
        
        /// @brief Very bouncy, medium friction
        static const Material BouncyBall;
        /// @brief Extremely high friction, no bounce
        static const Material Mud;
        /// @brief High friction, no bounce
        static const Material Sand;
        /// @brief Absorbs impact (very low restitution)
        static const Material Sponge;
        /// @brief Moderate friction and low bounce
        static const Material Leather;
        /// @brief Moderate friction and restitution
        static const Material HumanBody;
    };
} // namespace LP