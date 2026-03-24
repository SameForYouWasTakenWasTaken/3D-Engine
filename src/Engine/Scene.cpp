#include <Engine/Scene.hpp>

#include "Components/Model.hpp"
#include "Engine/Systems/AssetManager.hpp"

/**
 * @brief Adds a layer to the scene and performs attachment setup.
 *
 * @param layer Shared pointer to the layer to add. The layer's scene
 *              association will be set to this scene and its OnAttach()
 *              lifecycle method will be invoked.
 */
void Scene::AddLayer(std::shared_ptr<Layer> layer)
{
    m_Layers.push_back(layer);
    layer->SetScene(this);
    layer->OnAttach();
}

/**
 * @brief Removes a layer from the scene, detaching it and clearing its scene pointer.
 *
 * If the layer is not present in the scene, a warning is logged and no changes are made.
 *
 * @param layer Shared pointer to the layer to remove from the scene. */
void Scene::RemoveLayer(std::shared_ptr<Layer> layer)
{
    auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if (it == m_Layers.end())
    {
        logger.AppendLogTag("SCENE", LogColors::GREEN);
        logger.AppendLogTag("LAYER", LogColors::MAGENTA);
        logger.LogWarning("Couldn't find layer to remove!");
        logger.DumpLogs();
        return;
    }
    layer->OnDetach();
    layer->SetScene(nullptr);
    m_Layers.erase(it);
}

/**
 * @brief Delegates drawing to each attached layer by invoking their OnDraw callback.
 *
 * Calls OnDraw() on every layer in the scene in stored order; the Scene itself does not
 * perform renderer submissions or camera setup.
 */
void Scene::Draw()
{
    for (auto& layer : m_Layers)
    {
        layer->OnDraw();
    }
}

/**
 * @brief Update the scene and its subsystems for a single time step.
 *
 * Calls OnUpdate(dt) on each attached layer and advances the CameraManager using
 * the scene's entity registry and the provided delta time.
 *
 * @param dt Time step in seconds. 
 */
void Scene::Update(float dt)
{
    for (auto& layer : m_Layers)
    {
        layer->OnUpdate(dt);
    }

    m_CameraManager.Update(registry, dt);
}

/**
 * @brief Propagates an event to the scene's layers and, if unhandled, to the scene manager.
 *
 * Dispatches the provided event to each layer in order by invoking their OnEvent handlers.
 * Propagation stops as soon as a handler sets the event's Handled flag. If no layer handles
 * the event, it is forwarded to the attached SceneManager.
 *
 * @param e Event to dispatch; handlers may set `e.Handled` to prevent further propagation.
 */
void Scene::OnEvent(Event& e)
{
    for (auto& layer : m_Layers)
    {
        layer->OnEvent(e);
        if (e.Handled)
            break;
    }
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

/**
 * @brief Called when the scene is detached.
 *
 * Default no-op implementation. Override in derived classes to perform teardown or cleanup when the scene is detached.
 */
void Scene::OnDetach()
{
}

/**
 * @brief Obtain the Renderer service's engine context associated with this scene.
 *
 * @return std::optional<EngineContext*> `EngineContext*` pointing to the renderer's engine context if available, `std::nullopt` otherwise.
 */
std::optional<EngineContext*> Scene::GetContext()
{
    auto* context = &Services::Get().GetService<Renderer>().m_EngineContext;
    if (context)
        return context;
    return std::nullopt;
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
