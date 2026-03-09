#include <Engine/Systems/LightManager.hpp>

LightID LightManager::CreateLight()
{
    Light light;
    LightID id = AddLight(light);
    return id;
}
LightID LightManager::AddLight(Light light)
{
    LightID id = m_NextLightID++;
    if (m_Lights.contains(id)) return -1;
    m_Lights.emplace(id, light);
    return id;
}

void LightManager::RemoveLight(LightID id)
{
    if (!m_Lights.contains(id)) return;
    m_Lights.erase(id);
}

Light* LightManager::GetLight(LightID id)
{
    const auto it = m_Lights.find(id);
    if (it == m_Lights.end()) return nullptr;
    return &it->second;
}

void LightManager::UpdateToShader(Shader* shader) const
{   
constexpr int MAX_LIGHTS = 16;

    int numLights = std::min(static_cast<int>(m_Lights.size()), MAX_LIGHTS);

    int count = 0;
    for (auto& light : m_Lights)
    {
        shader->SetVec3("lights[" + std::to_string(count) + "].position", light.second.position);
        shader->SetVec3("lights[" + std::to_string(count) + "].color", light.second.color);

        shader->SetVec3("lights[" + std::to_string(count) + "].ambient", light.second.ambient);
        shader->SetVec3("lights[" + std::to_string(count) + "].diffuse", light.second.diffuse);
        shader->SetVec3("lights[" + std::to_string(count) + "].specular", light.second.specular);

        ++count;
        if (count >= numLights)
            break;
    }

    shader->SetInt("numLights", count);
}
