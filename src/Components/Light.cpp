#include <Components/Light.hpp>

/**
 * @brief Identifies this light as a point light.
 *
 * @return LightType The enum value representing a point light (LightType::POINT).
 */
LightType PointLight::GetType() const
{return LightType::POINT;}

/**
 * @brief Identify the light as a directional light.
 *
 * @return LightType::DIRECTIONAL indicating this is a directional light.
 */
LightType DirectionalLight::GetType() const
{return LightType::DIRECTIONAL;}

/**
 * @brief Identifies this light as a spot light.
 *
 * @return LightType The enum value LightType::SPOT.
 */
LightType SpotLight::GetType() const
{return LightType::SPOT;}

/**
 * @brief Uploads this point light's properties to a shader's pointLights array slot.
 *
 * Populates the shader uniforms for pointLights[at] with this light's color, position,
 * attenuation constants (constant, linear, quadratic), and lighting components
 * (ambient, diffuse, specular).
 *
 * @param shader Target shader where uniforms will be written.
 * @param at Index in the shader's `pointLights` array to populate.
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
 * @brief Uploads this directional light's properties to a shader at a specific array index.
 *
 * Writes the light's color, direction, ambient, diffuse, and specular components to the shader
 * uniforms named "dirLights[<at>].color", "dirLights[<at>].direction", "dirLights[<at>].ambient",
 * "dirLights[<at>].diffuse", and "dirLights[<at>].specular".
 *
 * @param shader Target shader receiving the uniform values.
 * @param at Index in the shader's `dirLights` uniform array where the values will be written.
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
 * @brief Uploads this spot light's properties into the shader's spotLights array at the given index.
 *
 * Populates shader uniforms for color, direction, position, cutoff angles (converted from degrees to cosine), maximum distance, attenuation (constant, linear, quadratic),
 * and lighting components (ambient, diffuse, specular) for the element spotLights[at].
 *
 * @param shader Shader to receive the uniform values.
 * @param at Index into the shader's spotLights uniform array where the light data will be written.
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
