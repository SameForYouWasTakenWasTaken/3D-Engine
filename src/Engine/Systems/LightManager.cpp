#include <ranges>
#include <Engine/Systems/LightManager.hpp>

std::optional<LightID> LightManager::CreateLight()
{
    Light light;
    LightID id = AddLight(light).value();
    return id;
}
std::optional<LightID> LightManager::AddLight(Light light)
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

    const int numLights = std::min(static_cast<int>(m_Lights.size()), MAX_LIGHTS);

    int count = 0;
    for (const auto& val : m_Lights | std::views::values)
    {
        shader->SetVec3("lights[" + std::to_string(count) + "].position", val.position);
        shader->SetVec3("lights[" + std::to_string(count) + "].color", val.color);

        shader->SetVec3("lights[" + std::to_string(count) + "].ambient", val.ambient);
        shader->SetVec3("lights[" + std::to_string(count) + "].diffuse", val.diffuse);
        shader->SetVec3("lights[" + std::to_string(count) + "].specular", val.specular);

        ++count;
        if (count >= numLights)
            break;
    }

    shader->SetInt("numLights", count);
}
