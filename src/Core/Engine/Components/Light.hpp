#pragma once
#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "Engine/Renderer/API/Shader.hpp"

using LightID = uint32_t;

enum class LightType
{
    NONE,
    DIRECTIONAL,
    POINT,
    SPOT,
};

// TODO: Fix light manager to get type
// TODO: Fix shaders
// TODO: Refactor unit tests

class DirectionalLight
{
public:
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f);

    float intensity = 1.0f;
    DirectionalLight() = default;
};


class PointLight
{
public:
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    // glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); //--> Handled by COMPTransform

    float intensity = 1.0f;
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f);

    PointLight() = default;
};

class SpotLight
{
public:
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    // glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // Handled by COMPTransform
    glm::vec3 direction = glm::vec3(0.f, 0.f, 0.f);

    float cosCutOff = std::cos(45.f);
    float cosOuterCutOff = std::cos(90.f);
    float dist = 0.0f;
    float intensity = 1.f;

    float constant = 1.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;

    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f);

    SpotLight() = default;
};

struct COMPLight 
{
    LightID id = static_cast<LightID>(-1);
    LightType type = LightType::NONE;
};