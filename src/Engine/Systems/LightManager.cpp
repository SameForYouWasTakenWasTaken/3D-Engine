#include <Engine/Systems/LightManager.hpp>

void LightManager::RemoveLight(LightID id)
{
    m_Lights.erase(id);
}

// TODO: Complete
void LightManager::UploadToShader(Shader* shader)
{
    constexpr int MAX_LIGHTS = 16;

    int countDir = 0;
    int countPoint = 0;
    int countSpot = 0;

    for (auto& [id, light] : m_Lights)
    {
        auto base = light.get();

        switch (base->GetType())
        {
        case LightType::DIRECTIONAL:
            if (countDir >= MAX_LIGHTS) continue; // Max lights?
            base->Upload(shader, countDir++);
            break;

        case LightType::POINT:
            if (countPoint >= MAX_LIGHTS) continue; // Max lights?
            base->Upload(shader, countPoint++);
            break;

        case LightType::SPOT:
            if (countSpot >= MAX_LIGHTS) continue; // Max lights?
            base->Upload(shader, countSpot++);
            break;

        default:
            break;
        }
    }

    shader->SetInt("numDirLights", countDir);
    shader->SetInt("numPointLights", countPoint);
    shader->SetInt("numSpotLights", countSpot);
}