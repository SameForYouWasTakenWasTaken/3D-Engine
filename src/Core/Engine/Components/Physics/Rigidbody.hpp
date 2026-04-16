#pragma once

#include "glm/vec3.hpp"

struct COMPRigidBody
{
    float mass = 1.0f;
    bool isStatic = false;
    bool useGravity = true;

    glm::vec3 linearVelocity{0.0f};
    glm::vec3 angularVelocity{0.0f};

    bool dirty = true;
};
