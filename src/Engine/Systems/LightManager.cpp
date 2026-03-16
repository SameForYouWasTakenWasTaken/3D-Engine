#include <Engine/Systems/LightManager.hpp>

/**
 * @brief Remove the light with the given identifier from the manager.
 *
 * Removes the light associated with the provided LightID from the internal
 * collection. If no light exists for the given id, the call has no effect.
 *
 * @param id Identifier of the light to remove.
 */
void LightManager::RemoveLight(LightID id)
{
    m_Lights.erase(id);
}

/**
 * @brief Uploads managed lights into the given shader and records per-type counts.
 *
 * Uploads up to 16 lights per light type (directional, point, spot) by invoking each light's
 * Upload(shader, index) with a sequential index for its type, and then writes the resulting
 * counts into the shader uniforms "numDirLights", "numPointLights", and "numSpotLights".
 *
 * @param shader Pointer to the shader that will receive per-light data and the three count uniforms.
 */
void LightManager::UploadToShader(Shader* shader)
{
    int countDir = 0;
    int countPoint = 0;
    int countSpot = 0;

    for (auto& [id, light] : m_Lights)
    {
        constexpr int MAX_LIGHTS = 16;
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