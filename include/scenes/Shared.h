#pragma once

#include <collision/OBBCollider.h>
#include <collision/BoundedPlaneCollider.h>

extern std::shared_ptr<LP::OBBCollider> cubeOBBCollider;
extern std::shared_ptr<LP::BoundedPlaneCollider> planeYUPCollider;

namespace SharedScene
{
    void createPlayer();
    void createPortals();
} // namespace SharedScene