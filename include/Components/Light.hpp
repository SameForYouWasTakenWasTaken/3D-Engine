#pragma once
#include <glm/glm.hpp>

struct COMPLight 
{
    glm::vec3 position = {0.f, 0.f, 0.f};
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
};