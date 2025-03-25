#include "dynamics/Material.h"

// Material instances
const LP::Material LP::Material::Ice       = { 0.1f, 0.05f, 0.02f };
const LP::Material LP::Material::Wood      = { 0.3f, 0.5f, 0.4f };
const LP::Material LP::Material::Rubber    = { 0.8f, 1.0f, 0.9f };
const LP::Material LP::Material::Metal     = { 0.4f, 0.2f, 0.15f };
const LP::Material LP::Material::Concrete  = { 0.1f, 0.9f, 0.7f };
const LP::Material LP::Material::Glass     = { 0.6f, 0.2f, 0.15f };
const LP::Material LP::Material::Carpet    = { 0.05f, 0.9f, 0.8f };
const LP::Material LP::Material::Plastic   = { 0.4f, 0.5f, 0.4f };

// Specialized
const LP::Material LP::Material::BouncyBall = { 0.9f, 0.6f, 0.5f };
const LP::Material LP::Material::Mud        = { 0.05f, 1.0f, 0.9f };
const LP::Material LP::Material::Sand       = { 0.1f, 0.8f, 0.6f };
const LP::Material LP::Material::Sponge     = { 0.2f, 0.4f, 0.3f };
const LP::Material LP::Material::Leather    = { 0.3f, 0.6f, 0.5f };
const LP::Material LP::Material::HumanBody  = { 0.5f, 0.4f, 0.5f };