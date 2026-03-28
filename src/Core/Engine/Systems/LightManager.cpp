#include <ranges>
#include "LightManager.hpp"

#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

/**
 * @brief Removes the light with the given identifier from the manager.
 *
 * Does nothing if no light exists for the provided `id`.
 *
 * @param id Identifier of the light to remove.
 */
void LightManager::RemoveLight(LightID id)
{
    auto it = m_Lights.find(id);
    if (it == m_Lights.end()) return;

    m_Lights.erase(id);
    dirty_SSBO = true;
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
void LightManager::UploadGPUData(Shader* shader, Mesh* mesh)
{
    if (!shader || !dirty_SSBO) return;

    std::vector<DirectionLightGPU> dirLights;
    std::vector<PointLightGPU> pointLights;
    std::vector<SpotLightGPU> spotLights;

    // TODO: Stop dynamic casting lel
    for (std::unique_ptr<LightBase>& light : m_Lights | std::views::values)
    {
        if (light->GetType() == LightType::DIRECTIONAL)
            dirLights.push_back(dynamic_cast<DirectionalLight*>(light.get())->data);

        else if (light->GetType() == LightType::SPOT)
            spotLights.push_back(dynamic_cast<SpotLight*>(light.get())->data);

        else if (light->GetType() == LightType::POINT)
            pointLights.push_back(dynamic_cast<PointLight*>(light.get())->data);
    }

    UploadLights<DirectionLightGPU>(dirLights, dirLightSSBO);
    UploadLights<PointLightGPU>(pointLights, pointLightSSBO);
    UploadLights<SpotLightGPU>(spotLights, spotLightSSBO);

    dirLightSSBO.SetBinding(2);
    pointLightSSBO.SetBinding(3);
    spotLightSSBO.SetBinding(4);

    shader->SetInt("numDirLights", static_cast<int>(dirLights.size()));
    shader->SetInt("numPointLights", static_cast<int>(pointLights.size()));
    shader->SetInt("numSpotLights", static_cast<int>(spotLights.size()));
    dirty_SSBO = false;
}
