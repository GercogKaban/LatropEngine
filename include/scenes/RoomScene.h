#pragma once

#include <glm/vec3.hpp>

namespace RoomScene
{
    void createFloor(int width = 20, int depth = 20);

    void createWall(glm::ivec2 normal, int width = 20, int height = 10);

    void createSmoothStairs(int height, glm::vec3 origin, float radius = 2.0f, float YStep = 0.1f, float angleStep = 0.1f);

    void createPerfectlyBouncyPuddle(glm::vec3 origin, bool withCube = false);

    void create();
} // namespace RoomScene