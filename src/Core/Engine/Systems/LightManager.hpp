#pragma once

#include <unordered_map>
#include <optional>

#include "API_Util/GPUData.hpp"
#include "Engine/Components/Light.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/Renderer/API/Shader.hpp"
#include "vendor/entt.hpp"


class LightManager final
{
    SSBO pointLightSSBO;
    SSBO dirLightSSBO;
    SSBO spotLightSSBO;

    bool dirty_Point = false;
    bool dirty_Direction = false;
    bool dirty_Spot = false;

    const entt::registry& m_SceneRegistry;

    static DirectionLightGPU ConvertLightToData(const DirectionalLight& light);
    static SpotLightGPU ConvertLightToData(const SpotLight& light);
    static PointLightGPU ConvertLightToData(const PointLight& light);

    template <typename Light, typename GPUData>
    requires(std::is_class_v<GPUData>)
    size_t UpdateLightGPUData(SSBO& ssbo); // Returns number of the lights
public:
    /**
 * @brief Constructs an empty LightManager.
 *
 * Initializes the internal light storage as empty and sets the next light ID counter to zero.
 */
LightManager(entt::registry& scene_registry);
    /**
 * @brief Deleted copy constructor to prevent copying of the manager.
 *
 * Prevents copying of a LightManager to preserve unique ownership of its stored lights and avoid accidental duplication
 * or double-deletion of resources.
 */
LightManager(const LightManager&) = delete;

    template <typename T>
    /**
     * @brief Retrieve a stored light by ID cast to the requested derived light type.
     *
     * @tparam T Desired light type to retrieve (must derive from LightBase).
     * @param id Identifier of the light to look up.
     * @return T* Pointer to the light cast to `T` if a light with `id` exists and is of type `T`, `nullptr` otherwise.
     */
    T* GetLight(entt::entity id)
    {
        T* light = m_SceneRegistry.try_get<T>(id);
        return light;
    }
    void UploadGPUData(Shader* shader);

    void OnSpotlightChanged();
    void OnPointLightChanged();
    void OnDirectionalLightChanged();
};

template <typename Light, typename GPUData> requires (std::is_class_v<GPUData>)
size_t LightManager::UpdateLightGPUData(SSBO& ssbo)
{
    auto lights = m_SceneRegistry.view<Light>();

    std::vector<GPUData> lightGPUData;

    size_t lightSize = lights.size();
    lightGPUData.reserve(lightSize);

    for (auto entity : lights)
    {
        auto& light = lights.template get<Light>(entity);
        GPUData data = ConvertLightToData(light);

        if constexpr (!std::is_same_v<Light, DirectionalLight>)
        {
            const COMPTransform* transform = m_SceneRegistry.try_get<COMPTransform>(entity);
            if (!transform) continue;

            // This is dangerous, because if you change the variable name to something
            // else, then this will be UB
            data.position = transform->WorldMatrix[3];
        }

        lightGPUData.push_back(std::move(data));
    }

    ssbo.UploadGPUData<GPUData>(lightGPUData);

    return lightGPUData.size();
}
