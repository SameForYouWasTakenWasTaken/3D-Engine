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
    (*it)->OnDetach();
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
            "PVmat", 
            1, 
            glm::value_ptr(cam->projection * cam->view)
        );
        material.shader->UnuseProgram();

        // Calculate the model
        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, transform.position);

        model = glm::rotate(model, transform.rotation.x, glm::vec3(1.f, 0.f, 0.f));
        model = glm::rotate(model, transform.rotation.y, glm::vec3(0.f, 1.f, 0.f));
        model = glm::rotate(model, transform.rotation.z, glm::vec3(0.f, 0.f, 1.f));
        
        model = glm::scale(model, transform.scale);
        // Put everything into the renderer
        m_SceneManager->m_EngineContext.renderer->Submit(mesh.mesh.get(), &material, model);
    });
}

void Scene::Update(float dt)
{
    m_CameraManager.Update(registry, dt);
    for (auto& layer : m_Layers)
    {
        layer->OnUpdate(dt);
    }
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

void Scene::OnAttach(SceneManager* sceneManager)
{
    if (sceneManager == nullptr)
        return;
    
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