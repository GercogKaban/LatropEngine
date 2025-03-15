#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace LatropPhysics 
{

struct Transform 
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};

} // namespace LatropPhysics