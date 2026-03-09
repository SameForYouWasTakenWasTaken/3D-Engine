#pragma once
#include <glm/glm.hpp>

using LightID = uint32_t;

struct Light
{
    glm::vec3 position = {0.f, 0.f, 0.f};
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specular = glm::vec3(.5f, .5f, .5f);
    glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    float intensity = .5f;
};

struct COMPLight 
{
    LightID id = static_cast<LightID>(-1);
};