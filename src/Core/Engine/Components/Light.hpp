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

struct alignas(16) LightBaseGPU
{
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
};

struct alignas(16) DirectionLightGPU : LightBaseGPU
{
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f); float _pad1 = 0.0f;
    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);  float _pad2 = 0.0f;
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;  float _pad3 = 0.0f;
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f); float _pad4 = 0.0f;
};

struct alignas(16) PointLightGPU : LightBaseGPU
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); float _pad1 = 0.0f;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float _pad2 = 0.0f;

    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);  float _pad3 = 0.0f;
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;  float _pad4 = 0.0f;
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f); float _pad5 = 0.0f;
};
 // TODO: Fix light
struct alignas(16) SpotLightGPU : LightBaseGPU
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); float _pad0 = 0.0f;
    glm::vec3 direction = glm::vec3(0.f, 0.f, 0.f); float _pad1 = 0.0f;

    float cosCutOff = cos(45);
    float cosOuterCutOff = cos(90.f);
    float dist = 0.0f;
    float _pad3 = 0.0f;

    float constant = 1.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;
    float _pad4 = 0.0f;

    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);  float _pad5= 0.0f;
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;  float _pad6 = 0.0f;
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f); float _pad7 = 0.0f;
};


class LightBase
{
public:
    /**
 * @brief Ensures derived light objects are cleaned up correctly when deleted via a base pointer.
 *
 * Defaulted virtual destructor to allow proper polymorphic destruction of LightBase-derived instances.
 */
    virtual ~LightBase() = default;
    [[nodiscard]] virtual LightType GetType() const = 0;
    float intensity = 0.5f;
};

class DirectionalLight : public LightBase
{
public:
    DirectionLightGPU data;
    [[nodiscard]] LightType GetType() const override;
    DirectionalLight() = default;
};


class PointLight : public LightBase
{
public:
    PointLightGPU data;
    [[nodiscard]] LightType GetType() const override;
    PointLight() = default;
};

class SpotLight : public LightBase
{
public:
    SpotLightGPU data;

    [[nodiscard]] LightType GetType() const override;
    SpotLight() = default;
};

struct COMPLight 
{
    LightID id = static_cast<LightID>(-1);
    LightType type = LightType::NONE;
};