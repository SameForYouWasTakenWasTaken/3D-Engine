#include "Light.hpp"

/**
 * @brief Get the light type for this instance.
 *
 * @return LightType::POINT
 */
LightType PointLight::GetType() const
{return LightType::POINT;}

/**
 * @brief Identifies the light as a directional light.
 *
 * @return LightType::DIRECTIONAL The enum value representing a directional light.
 */
LightType DirectionalLight::GetType() const
{return LightType::DIRECTIONAL;}

/**
 * @brief Identifies this light as a spot light.
 *
 * @return LightType::SPOT The spot light type.
 */
LightType SpotLight::GetType() const
{return LightType::SPOT;}

/**
 * @brief Uploads this point light's properties into the given shader's pointLights array at the specified index.
 *
 * Writes the light's color, position, attenuation terms (`constant`, `linear`, `quadratic`) and lighting components
 * (`ambient`, `diffuse`, `specular`) to uniforms named "pointLights[<at>].<field>".
 *
 * @param shader Target shader to receive the uniforms.
 * @param at Index in the shader's `pointLights` uniform array.
 */
void PointLight::Upload(Shader* shader, const int at) const
{
    shader->SetVec3("pointLights[" + std::to_string(at) + "].color", color);
    shader->SetVec3("pointLights[" + std::to_string(at) + "].position", position);

    shader->SetFloat("pointLights[" + std::to_string(at) + "].constant", constant);
    shader->SetFloat("pointLights[" + std::to_string(at) + "].linear", linear);
    shader->SetFloat("pointLights[" + std::to_string(at) + "].quadratic", quadratic);

    shader->SetVec3("pointLights[" + std::to_string(at) + "].ambient", ambient);
    shader->SetVec3("pointLights[" + std::to_string(at) + "].diffuse", diffuse);
    shader->SetVec3("pointLights[" + std::to_string(at) + "].specular", specular);
}


/**
 * @brief Uploads this directional light's properties to a shader.
 *
 * Writes the light's color, direction, and ambient/diffuse/specular components into the
 * shader uniforms for dirLights[at].
 *
 * @param shader Target shader to receive the uniform values.
 * @param at Index inside the shader's `dirLights` uniform array where the values are written.
 */
void DirectionalLight::Upload(Shader* shader, const int at) const
{
    shader->SetVec3("dirLights[" + std::to_string(at) + "].color", color);
    shader->SetVec3("dirLights[" + std::to_string(at) + "].direction", direction);
    shader->SetVec3("dirLights[" + std::to_string(at) + "].ambient", ambient);
    shader->SetVec3("dirLights[" + std::to_string(at) + "].diffuse", diffuse);
    shader->SetVec3("dirLights[" + std::to_string(at) + "].specular", specular);
}

/**
 * @brief Uploads this spot light's parameters into the given shader at the specified spotLights array index.
 *
 * Uploads color, direction, position, attenuation (constant, linear, quadratic), lighting terms (ambient, diffuse, specular),
 * distance, and cutoff values; `cutOff` and `outerCutOff` are uploaded as the cosine of their angles (degrees).
 *
 * @param shader Target shader that will receive the uniforms.
 * @param at Index into the shader's `spotLights` uniform array where values will be written.
 */
void SpotLight::Upload(Shader* shader, int at) const
{
    shader->SetVec3("spotLights[" + std::to_string(at) + "].color", color);
    shader->SetVec3("spotLights[" + std::to_string(at) + "].direction", direction);
    shader->SetVec3("spotLights[" + std::to_string(at) + "].position", position);

    float cutOffCos = cos(glm::radians(cutOff));
    float outerCutOffCos = cos(glm::radians(outerCutOff));

    shader->SetFloat("spotLights[" + std::to_string(at) + "].cutOff", cutOffCos);
    shader->SetFloat("spotLights[" + std::to_string(at) + "].outerCutOff", outerCutOffCos);
    shader->SetFloat("spotLights[" + std::to_string(at) + "].dist", distance);

    shader->SetFloat("spotLights[" + std::to_string(at) + "].constant", constant);
    shader->SetFloat("spotLights[" + std::to_string(at) + "].linear", linear);
    shader->SetFloat("spotLights[" + std::to_string(at) + "].quadratic", quadratic);

    shader->SetVec3("spotLights[" + std::to_string(at) + "].ambient", ambient);
    shader->SetVec3("spotLights[" + std::to_string(at) + "].diffuse", diffuse);
    shader->SetVec3("spotLights[" + std::to_string(at) + "].specular", specular);
}
