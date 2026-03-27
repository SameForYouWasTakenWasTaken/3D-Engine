#include "Scene.hpp"

#include "Engine/Engine.hpp"
#include "Engine/Components/Model.hpp"
#include "Engine/Systems/AssetManager.hpp"
#include "tracy/Tracy.hpp"

Scene::Scene()
{}

void Scene::Update(float dt)
{
    auto& context = m_SceneContext;
    context.m_CameraManager.Update(context.registry, dt);
}

/**
 * @brief Set the scene's identifier.
 *
 * Assigns the provided numeric id to this Scene's SceneID.
 *
 * @param id The numeric identifier to assign to the scene.
 */
void Scene::OnAttach(uint32_t id)
{
    SceneID = id;
}

void Scene::OnDraw()
{
    for (auto& layer : m_Layers)
        layer->OnDraw();
}

void Scene::OnUpdate(float dt)
{
    auto& context = m_SceneContext;
    for (auto& layer : m_Layers)
        layer->OnUpdate(dt);

    context.m_CameraManager.Update(context.registry, dt);
}

/**
 * @brief Obtain the Renderer service's engine context associated with this scene.
 *
 * @return std::optional<EngineContext*> `EngineContext*` pointing to the renderer's engine context if available, `std::nullopt` otherwise.
 */
SceneContext& Scene::GetContext()
{
    return m_SceneContext;
}

/**
 * @brief Retrieves the scene's identifier.
 *
 * @return uint32_t The numeric identifier assigned to this scene.
 */
uint32_t Scene::GetID() const
{
    return SceneID;
}
