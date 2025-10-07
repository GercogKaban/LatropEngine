#pragma once

#include <collision/OBBCollider.h>
#include <collision/BoundedPlaneCollider.h>
#include <string>

extern std::shared_ptr<LP::OBBCollider> cubeOBBCollider;
extern std::shared_ptr<LP::BoundedPlaneCollider> planeYUPCollider;

namespace SharedScene
{
    void createPlayer(glm::vec3 origin = glm::vec3(2.0f, 2.0f, 2.0f), bool takesGravity = true);
    void createPortals();

    void createCube(glm::vec3 origin, bool takesGravity = true, std::__1::string &&texturePath = "textures/smile2.jpg");
    void createBluePortalUI(glm::vec3 origin, glm::quat rotation, glm::vec3 scale = { 1.6f, 0.01f, 2.8f }, bool isDecorated = false);
    void createOrangePortalUI(glm::vec3 origin, glm::quat rotation, glm::vec3 scale = { 1.6f, 0.01f, 2.8f }, bool isDecorated = false);
} // namespace SharedScene