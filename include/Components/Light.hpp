#pragma once
#include <glm/glm.hpp>
#include <Engine/LowLevel/Shader.hpp>

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

class LightBase
{
public:
    /**
 * @brief Virtual destructor to allow proper cleanup of derived light objects.
 *
 * Ensures that destroying a LightBase pointer invokes the destructor of the concrete derived class.
 */
virtual ~LightBase() = default;
    virtual void Upload(Shader* shader, int at) const = 0;
    [[nodiscard]] virtual LightType GetType() const = 0;

    glm::vec3 color = {1.f, 1.f, 1.f};
    float intensity = 0.5f;
};

class DirectionalLight : public LightBase
{
public:
    glm::vec3 direction = {0.f, 0.f, 0.f};

    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specular = glm::vec3(.5f, .5f, .5f);
    glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);

    [[nodiscard]] LightType GetType() const override;
    void Upload(Shader* shader, int at) const override;
};


class PointLight : public LightBase
{
public:
    glm::vec3 position = {0.f, 0.f, 0.f};

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    glm::vec3 diffuse  = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 ambient  = glm::vec3(0.05f, 0.05f, 0.05f);

    [[nodiscard]] LightType GetType() const override;
    void Upload(Shader* shader, int at) const override;
};

class SpotLight : public PointLight
{
public:
    glm::vec3 direction = {0.f, 0.f, 0.f};

    float distance = 3.f;
    float cutOff = 30.f; // inner cone angle, in degrees
    float outerCutOff = 40.f; //  outer cone angle, should be larger than cutoff, in degrees

    [[nodiscard]] LightType GetType() const override;
    void Upload(Shader* shader, int at) const override;
};

struct COMPLight 
{
    LightID id = static_cast<LightID>(-1);
    LightType type = LightType::NONE;
};