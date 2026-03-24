#include <Engine/Systems/LightManager.hpp>

/**
 * @brief Removes the light with the given identifier from the manager.
 *
 * Does nothing if no light exists for the provided `id`.
 *
 * @param id Identifier of the light to remove.
 */
void LightManager::RemoveLight(LightID id)
{
    m_Lights.erase(id);
}

/**
 * @brief Uploads managed lights to the given shader, organizing them by type.
 *
 * Iterates all lights held by the manager and calls each light's Upload(shader, index)
 * with an index within its type category. Limits each category (directional, point, spot)
 * to a maximum of 16 uploaded lights and writes the final per-type counts into the
 * shader uniforms "numDirLights", "numPointLights", and "numSpotLights".
 *
 * @param shader Target shader to receive light uniforms. If `nullptr`, the function does nothing.
 */
void LightManager::UploadToShader(Shader* shader)
{
    if (!shader) return;

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