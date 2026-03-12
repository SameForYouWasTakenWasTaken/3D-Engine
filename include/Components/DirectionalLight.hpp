#pragma once
#include <glm/glm.hpp>

using LightID = uint32_t;

enum class LightType
{
    NONE,
    DIRECTIONAL,
    POINT,
};

// TODO: Fix light manager to get type
// TODO: Fix shaders
// TODO: Refactor unit tests

struct LightBase
{
    glm::vec3 color = {1.f, 1.f, 1.f};
    float intensity = 0.5f;

    LightType type = LightType::NONE;
};

struct DirectionalLight
{
    glm::vec3 direction = {0.f, 0.f, 0.f};

    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specular = glm::vec3(.5f, .5f, .5f);
    glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);

    LightType type = LightType::DIRECTIONAL;
};

struct PointLight
{

};

struct COMPLight 
{
    LightID id = static_cast<LightID>(-1);
    LightType type = LightType::NONE;
};