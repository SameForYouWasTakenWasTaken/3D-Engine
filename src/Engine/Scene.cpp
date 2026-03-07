#include <Engine/Scene.hpp>

void Scene::AddLayer(std::shared_ptr<Layer> layer)
{
    m_Layers.push_back(layer);
    layer->SetScene(this);
    layer->OnAttach();
}

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

void Scene::Draw()
{

    for (auto& layer : m_Layers)
    {
        layer->OnDraw();
    }

    auto view = registry.view<
    COMPGeometry,
    COMPMesh,
    COMPMaterial,
    COMPTransform
    >();

    auto* cam = registry.try_get<COMPCamera>(m_CameraManager.GetActiveCamera());
    if (cam == nullptr)
    {
        logger.AppendLogTag("SCENE", LogColors::GREEN);
        logger.AppendLogTag("CAMERA_MANAGER", LogColors::YELLOW);
        logger.LogWarning("No active camera set, skipping draw.");
        return;
    }

    view.each([&](auto entity, 
        COMPGeometry& drawable, 
        COMPMesh& mesh, 
        COMPMaterial& material, 
        COMPTransform& transform)
    {
        material.shader->UseProgram();
        material.shader->SetMatrix4(
            "projectmat", 
            1, 
            glm::value_ptr(cam->projection)
        );
        material.shader->SetMatrix4(
            "viewmat", 
            1, 
            glm::value_ptr(cam->view)
        );
        material.shader->UnuseProgram();

        // Put everything into the renderer
        m_SceneManager->m_EngineContext.renderer->Submit(mesh.mesh.get(), &material, transform.GetModelMatrix());
    });
}

void Scene::Update(float dt)
{
    for (auto& layer : m_Layers)
    {
        layer->OnUpdate(dt);
    }
    m_CameraManager.Update(registry, dt);
}

void Scene::OnEvent(Event& e)
{
    for (auto& layer : m_Layers)
    {
        layer->OnEvent(e);
        if (e.Handled)
            break;
    }

    if (!e.Handled)
        m_SceneManager->OnEvent(e);
}

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

uint32_t Scene::GetID() const
{
    return SceneID;
}