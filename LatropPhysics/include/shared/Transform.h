#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

namespace LatropPhysics 
{

struct Transform 
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::quat rotation;

    glm::mat4 getAsMatrix() const 
    {
        // Start with an identity matrix
        glm::mat4 mat = glm::mat4(1.0f);
    
        // Apply translation
        mat = glm::translate(mat, position);
    
        // Apply rotation (convert quaternion to matrix)
        mat *= glm::mat4_cast(rotation);
    
        // Apply scaling
        mat = glm::scale(mat, scale);
    
        return mat;
    }
};

} // namespace LatropPhysics