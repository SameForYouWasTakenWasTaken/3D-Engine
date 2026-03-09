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
 * @brief Renders the scene by drawing layers and submitting visible entities to the renderer.
 *
 * Calls OnDraw() on every attached layer, then iterates over entities that have geometry, mesh,
 * material, and transform components. For each such entity, the material's shader receives the
 * active camera's projection and view matrices as the "projectmat" and "viewmat" uniforms, and
 * the mesh, material, and entity model matrix are submitted to the engine renderer.
 *
 * If no active camera is set, a warning is logged and the draw is skipped.
 */
void Scene::Draw()
{

    for (auto& layer : m_Layers)
    {
        layer->OnDraw();
    }

    auto* cam = registry.try_get<COMPCamera>(m_CameraManager.GetActiveCamera());
    auto* camTransform = registry.try_get<COMPTransform>(m_CameraManager.GetActiveCamera());
    if (cam == nullptr || camTransform == nullptr)
    {
        logger.AppendLogTag("SCENE", LogColors::GREEN);
        logger.AppendLogTag("CAMERA_MANAGER", LogColors::YELLOW);
        logger.LogWarning("No active camera set, skipping draw.");
        return;
    }
    auto& context = m_SceneManager->m_EngineContext;
    context.cached_projection = cam->projection;
    context.cached_view = cam->view;
    context.cached_activeCam_position = camTransform->position;

    auto view = registry.view<
    COMPGeometry,
    COMPMesh,
    COMPMaterial,
    COMPTransform
    >();



    auto renderer = m_SceneManager->m_EngineContext.renderer;
    if (!renderer)
    {
        logger.AppendLogTag("SCENE", LogColors::GREEN);
        logger.AppendLogTag("ENGINE_CONTEXT", LogColors::YELLOW);
        logger.LogWarning("No renderer set, skipping draw.");
        return;
    }
    view.each([&](auto entity, 
        COMPGeometry& drawable, 
        COMPMesh& mesh, 
        COMPMaterial& material, 
        COMPTransform& transform)
    {
        // Put everything into the renderer
       renderer->Submit(
        mesh.mesh.get(), 
        material.material, 
        transform.GetModelMatrix(), std::make_shared<LightManager>(m_LightManager));
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
 * @brief Attach the scene to a SceneManager and assign the scene's identifier.
 *
 * Associates this Scene with the given SceneManager and sets the SceneID to `id`.
 *
 * @param sceneManager Pointer to the SceneManager to attach; if `nullptr`, the scene is not attached.
 * @param id Numeric identifier to assign to this scene.
 */
void Scene::OnAttach(SceneManager* sceneManager, uint32_t id)
{
    if (sceneManager == nullptr)
        return;

    SceneID = id;
    m_SceneManager = sceneManager;
}

void Scene::OnDetach()
{
    m_SceneManager = nullptr;
}

/**
 * @brief Retrieve the scene's engine context when the scene is attached to a SceneManager.
 *
 * If the scene has not been attached to a SceneManager, a warning is logged and logs are dumped.
 *
 * @return EngineContext* Pointer to the EngineContext when attached; otherwise an unexpected value (`false`) indicating no context is available.
 */
std::expected<EngineContext*, bool> Scene::GetContext()
{
    if (m_SceneManager == nullptr)
    {
        logger.AppendLogTag("SCENE", LogColors::GREEN);
        logger.AppendLogTag("SCENE_MANAGER", LogColors::MAGENTA);
        logger.LogWarning("Scene has not been attached to a scene manager.");
        logger.DumpLogs();
        return std::unexpected(false);
    }
    return &m_SceneManager->m_EngineContext;
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