#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Collider.h"

struct CollisionBody
{
protected:
    Collider* collider;
    Transform* transform;

    bool isDynamic;
    bool isTrigger;
public:
};