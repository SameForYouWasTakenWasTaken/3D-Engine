#include <Components/Light.hpp>

LightType PointLight::GetType() const
{return LightType::POINT;}

LightType DirectionalLight::GetType() const
{return LightType::DIRECTIONAL;}

LightType SpotLight::GetType() const
{return LightType::SPOT;}

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


void DirectionalLight::Upload(Shader* shader, const int at) const
{
    shader->SetVec3("dirLights[" + std::to_string(at) + "].color", color);
    shader->SetVec3("dirLights[" + std::to_string(at) + "].direction", direction);
    shader->SetVec3("dirLights[" + std::to_string(at) + "].ambient", ambient);
    shader->SetVec3("dirLights[" + std::to_string(at) + "].diffuse", diffuse);
    shader->SetVec3("dirLights[" + std::to_string(at) + "].specular", specular);
}

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
