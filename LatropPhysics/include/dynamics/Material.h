#pragma once

namespace LP
{
    struct Material
    {
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