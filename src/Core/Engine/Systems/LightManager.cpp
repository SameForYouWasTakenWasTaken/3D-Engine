#include <ranges>
#include "LightManager.hpp"

#include "Engine/Components/Transform.hpp"
#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

DirectionLightGPU LightManager::ConvertLightToData(const DirectionalLight& light)
{
    return DirectionLightGPU{
        .color = light.color,
        .intensity  = light.intensity,
        .direction = light.direction,
        .ambient = light.ambient,
        .diffuse = light.diffuse,
        .specular = light.specular,
    };
}

SpotLightGPU LightManager::ConvertLightToData(const SpotLight& light)
{
    return SpotLightGPU
    {
        .color = light.color,
        .intensity  = light.intensity,
        .direction = light.direction,
        .cosCutOff = light.cosCutOff,
        .cosOuterCutOff = light.cosOuterCutOff,
        .dist = light.dist,
        .constant =  light.constant,
        .linear = light.linear,
        .quadratic = light.quadratic,
        .ambient = light.ambient,
        .diffuse = light.diffuse,
        .specular = light.specular
    };
}

PointLightGPU LightManager::ConvertLightToData(const PointLight& light)
{
    return PointLightGPU{
        .color = light.color,
        .intensity  = light.intensity,
        .constant = light.constant,
        .linear = light.linear,
        .quadratic = light.quadratic,
        .ambient = light.ambient,
        .diffuse = light.diffuse,
        .specular = light.specular
    };
}

LightManager::LightManager(entt::registry& scene_registry)
    : m_SceneRegistry(scene_registry)
{
    scene_registry.on_construct<PointLight>().connect<&LightManager::OnPointLightChanged>(*this);
    scene_registry.on_destroy<PointLight>().connect<&LightManager::OnPointLightChanged>(*this);
    scene_registry.on_update<PointLight>().connect<&LightManager::OnPointLightChanged>(*this);

    scene_registry.on_construct<SpotLight>().connect<&LightManager::OnSpotlightChanged>(*this);
    scene_registry.on_destroy<SpotLight>().connect<&LightManager::OnSpotlightChanged>(*this);
    scene_registry.on_update<SpotLight>().connect<&LightManager::OnSpotlightChanged>(*this);

    scene_registry.on_construct<DirectionalLight>().connect<&LightManager::OnDirectionalLightChanged>(*this);
    scene_registry.on_destroy<DirectionalLight>().connect<&LightManager::OnDirectionalLightChanged>(*this);
    scene_registry.on_update<DirectionalLight>().connect<&LightManager::OnDirectionalLightChanged>(*this);

    // Fix on transform creation
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
void LightManager::UploadGPUData(Shader* shader)
{
    if (!shader) return;

    size_t dirLightSize{};
    size_t pointLightSize{};
    size_t spotLightSize{};

    if (dirty_Direction)
    {
        dirLightSize = UpdateLightGPUData<DirectionalLight, DirectionLightGPU>(dirLightSSBO);
        dirLightSSBO.SetBinding(2);
    }

    if (dirty_Point)
    {
        pointLightSize = UpdateLightGPUData<PointLight, PointLightGPU>(pointLightSSBO);
        pointLightSSBO.SetBinding(3);
    }

    if (dirty_Spot)
    {
        spotLightSize = UpdateLightGPUData<SpotLight, SpotLightGPU>(spotLightSSBO);
        spotLightSSBO.SetBinding(4);
    }

    dirty_Spot = false;
    dirty_Point = false;
    dirty_Direction = false;
    shader->SetInt("numDirLights", static_cast<int>(dirLightSize));
    shader->SetInt("numPointLights", static_cast<int>(pointLightSize));
    shader->SetInt("numSpotLights", static_cast<int>(spotLightSize));
}

void LightManager::OnSpotlightChanged()
{
    dirty_Spot = true;
}

void LightManager::OnPointLightChanged()
{
    dirty_Point = true;
}

void LightManager::OnDirectionalLightChanged()
{
    dirty_Direction = true;
}

