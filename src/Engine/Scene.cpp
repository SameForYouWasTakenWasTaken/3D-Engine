#include <Engine/Scene.hpp>

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
 * @brief Draws the scene by invoking layer draw callbacks and submitting renderable entities.
 *
 * Calls OnDraw() on every attached layer, updates the renderer engine context with the active
 * camera's projection, view, and camera position, then iterates entities with geometry, mesh,
 * material, and transform components and submits each mesh, material, model matrix, and the
 * scene's light manager to the renderer. If no active camera or its transform is available,
 * a warning is logged and drawing is skipped.
 */
void Scene::Draw()
{

    for (auto& layer : m_Layers)
    {
        layer->OnDraw();
    }

    auto* cam = registry.try_get<COMPCamera>(m_CameraManager.GetActiveCamera());
    auto* camTransform = registry.try_get<COMPTransform>(m_CameraManager.GetActiveCamera());
    auto& renderer = Services::Get().GetService<Renderer>();

    if (cam == nullptr || camTransform == nullptr)
    {
        logger.AppendLogTag("SCENE", LogColors::GREEN);
        logger.AppendLogTag("CAMERA_MANAGER", LogColors::YELLOW);
        logger.LogWarning("No active camera set, skipping draw.");
        return;
    }
    auto& context = renderer.m_EngineContext;
    context.cached_projection = cam->projection;
    context.cached_view = cam->view;
    context.cached_activeCam_position = camTransform->position;

    auto view = registry.view<
    COMPGeometry,
    COMPMesh,
    COMPMaterial,
    COMPTransform
    >();

    view.each([&](auto entity, 
        COMPGeometry& drawable, 
        COMPMesh& mesh, 
        COMPMaterial& material, 
        COMPTransform& transform)
    {
        // Put everything into the renderer
       renderer.Submit(
        mesh.mesh.get(), 
        material.material, 
        transform.GetModelMatrix(), m_LightManager);
    });
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
 * @brief Retrieve the Renderer service's EngineContext for this scene.
 *
 * @return std::expected<EngineContext*, bool> `EngineContext*` pointing to the Renderer service's engine context on success; `std::unexpected(false)` when the context is not available.
 */
std::expected<EngineContext*, bool> Scene::GetContext()
{
    return &Services::Get().GetService<Renderer>().m_EngineContext;
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