#include <Engine/Scene.hpp>

void Scene::AddLayer(std::shared_ptr<Layer> layer)
{
    m_Layers.push_back(layer);
    layer->OnAttach(*this, m_Registry);
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

    (*it)->OnDetach(m_Registry);
    m_Layers.erase(it);
}

void Scene::Draw(Renderer& renderer)
{
    for (auto& layer : m_Layers)
    {
        layer->OnDraw(*this, m_Registry);
    }

    auto view = m_Registry.view<
    COMPGeometry,
    COMPMesh,
    COMPMaterial,
    COMPTransform
    >();

    auto* cam = m_Registry.try_get<COMPCamera>(m_CameraManager.GetActiveCamera());
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
        renderer.Submit(mesh.mesh.get(), &material, transform);
    });
}

void Scene::Update(float dt)
{
    m_CameraManager.Update(m_Registry, dt);
    for (auto& layer : m_Layers)
    {
        layer->OnUpdate(m_Registry, dt);
    }
}

void Scene::OnEvent(Event& e)
{
    for (auto& layer : m_Layers)
    {
        layer->OnEvent(m_Registry, e);
        if (e.Handled)
            break;
    }

    if (auto* resizeEvent = dynamic_cast<WindowResizeEvent*>(&e))
    {
        auto activeCamera = m_CameraManager.GetActiveCamera();
        auto* cam = m_Registry.try_get<COMPCamera>(activeCamera);
        if (cam)
        {
            // Set aspect ratio to new size
            cam->SetRatio(
                static_cast<float>(resizeEvent->Width) / static_cast<float>(resizeEvent->Height)
            );
        }
    }
}