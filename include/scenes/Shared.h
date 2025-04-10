#pragma once

#include <collision/OBBCollider.h>
#include <collision/BoundedPlaneCollider.h>

extern std::shared_ptr<LP::OBBCollider> cubeOBBCollider;
extern std::shared_ptr<LP::BoundedPlaneCollider> planeYUPCollider;

namespace SharedScene
{
    void createPlayer(glm::vec3 origin = glm::vec3(2.0f, 2.0f, 2.0f), bool takesGravity = true);
    void createPortals();
} // namespace SharedScene