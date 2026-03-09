#include <Engine/Systems/LightManager.hpp>

uint32_t LightManager::CreateLight()
{
    COMPLight light;
    uint32_t id = AddLight(light);
    return id;
}
uint32_t LightManager::AddLight(COMPLight light)
{
    uint32_t id = m_NextLightID++;
    if (m_Lights.find(id) != m_Lights.end()) return -1;
    m_Lights.emplace(id, light);
    return id;
}

void LightManager::RemoveLight(uint32_t id)
{
    if (m_Lights.find(id) == m_Lights.end()) return;
    m_Lights.erase(id);
}

COMPLight* LightManager::GetLight(uint32_t id)
{
    auto it = m_Lights.find(id);
    if (it == m_Lights.end()) return nullptr;
    return &it->second;
}

void LightManager::UpdateToShader(Shader* shader)
{   
    constexpr int MAX_LIGHTS = 16;
    int numLights = std::min((int)m_Lights.size(), MAX_LIGHTS);
    
    shader->SetInt("numLights", numLights);

    int count = 0;
    for (auto& light : m_Lights)
    {
        ++count;
        shader->SetVec3("lightPositions[" + std::to_string(light.first) + "]", light.second.position);
        shader->SetVec3("lightColors[" + std::to_string(light.first) + "]", light.second.color);

        if (count >= numLights) break;
    }
}