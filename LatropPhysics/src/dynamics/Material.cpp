#include "dynamics/Material.h"
#include <cmath>

using namespace LP;

Material::CombinationMode max(Material::CombinationMode a, Material::CombinationMode b)
{
    return static_cast<int>(a) > static_cast<int>(b) ? a : b;
}

Material Material::combinedWith(const Material& otherMaterial) const
{
    Material::CombinationMode frictionMode = max(restitutionCombinator, otherMaterial.restitutionCombinator);
    Material::CombinationMode restitutionMode = max(restitutionCombinator, otherMaterial.restitutionCombinator);

    float combinedStaticFriction;
    float combinedDynamicFriction;
    float combinedRestituion;

    switch (frictionMode)
    {
    case Material::CombinationMode::Average:
        combinedStaticFriction = (staticFriction + otherMaterial.staticFriction) / 2.0f;
        combinedDynamicFriction = (dynamicFriction + otherMaterial.dynamicFriction) / 2.0f;
        break;
    case Material::CombinationMode::Minimum:
        combinedStaticFriction = std::fmin(staticFriction, otherMaterial.staticFriction);
        combinedDynamicFriction = std::fmin(dynamicFriction, otherMaterial.dynamicFriction);
        break;
    case Material::CombinationMode::Multiply:
        combinedStaticFriction = staticFriction * otherMaterial.staticFriction;
        combinedDynamicFriction = dynamicFriction * otherMaterial.dynamicFriction;
        break;
    case Material::CombinationMode::Maximum:
        combinedStaticFriction = std::fmax(staticFriction, otherMaterial.staticFriction);
        combinedDynamicFriction = std::fmax(dynamicFriction, otherMaterial.dynamicFriction);
        break;
    }

    switch (restitutionMode)
    {
    case Material::CombinationMode::Average:
        combinedRestituion = (restitution + otherMaterial.restitution) / 2.0f;
        break;
    case Material::CombinationMode::Minimum:
        combinedRestituion = std::fmin(restitution, otherMaterial.restitution);
        break;
    case Material::CombinationMode::Multiply:
        combinedRestituion = restitution * otherMaterial.restitution;
        break;
    case Material::CombinationMode::Maximum:
        combinedRestituion = std::fmax(restitution, otherMaterial.restitution);
        break;
    }

    return {
        combinedStaticFriction,
        combinedDynamicFriction,
        combinedRestituion,
        frictionMode,
        restitutionMode
    };
}

// Material instances
const Material Material::Ice       = { 0.1f, 0.05f, 0.02f };
const Material Material::Wood      = { 0.3f, 0.5f, 0.4f };
const Material Material::Rubber    = { 0.8f, 1.0f, 0.9f };
const Material Material::Metal     = { 0.4f, 0.2f, 0.15f };
const Material Material::Concrete  = { 0.1f, 0.9f, 0.7f };
const Material Material::Glass     = { 0.6f, 0.2f, 0.15f };
const Material Material::Carpet    = { 0.05f, 0.9f, 0.8f };
const Material Material::Plastic   = { 0.4f, 0.5f, 0.4f };

// Specialized
const Material Material::BouncyBall = { 0.9f, 0.6f, 0.5f };
const Material Material::Mud        = { 0.05f, 1.0f, 0.9f };
const Material Material::Sand       = { 0.1f, 0.8f, 0.6f };
const Material Material::Sponge     = { 0.2f, 0.4f, 0.3f };
const Material Material::Leather    = { 0.3f, 0.6f, 0.5f };
const Material Material::HumanBody  = { 0.5f, 0.4f, 0.05f };