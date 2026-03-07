#include <Engine/Layers/GameLayer.hpp>

void GameLayer::OnDraw()
{
    int x = 1;
}

void GameLayer::OnUpdate(float dt)
{
    m_Scene->registry.view<COMPTransform>().each([&](auto entity, COMPTransform& transform){
        transform.Rotate({0.f, 10.f * dt, 0.f});
    });
    
    TestEvent e{"Monkeys!"};
    EmitEvent(e);
}

void GameLayer::OnAttach()
{
    std::vector<Vertex> vertices = {
        {{-0.5f,0.f,0.f}, {1.f,0.f,0.f,1.f}, {1.f, 1.f}}, // bottom left
        {{0.f,0.5f,0.f}, {0.f,1.f,0.f,1.f}, {1.f, 1.f}}, // middle top
        {{0.5f,0.f,0.f}, {0.f,0.f,1.f,1.f}, {1.f, 1.f}} // bottom right
    };

    std::vector<GLuint> indices = {
        0,1,2
    };

    // shaders
    std::unique_ptr<Shader> shader = std::make_unique<Shader>("Shaders/first.vert", "Shaders/first.frag");
    std::unique_ptr<Shader> shader_b = std::make_unique<Shader>("Shaders/first.vert", "Shaders/first.frag");
    
    // first triangle
    auto entity = m_Scene->registry.create();
    auto& Geometry = m_Scene->registry.emplace<COMPGeometry>(entity, vertices, indices);
    auto& Transform = m_Scene->registry.emplace<COMPTransform>(entity);
    auto& Material = m_Scene->registry.emplace<COMPMaterial>(entity, std::move(shader), glm::vec3(1.f, 0.f, 0.f));
    m_Scene->registry.emplace<TAG_GameLayer>(entity);

    auto mesh = std::make_shared<Mesh>();
    mesh->SetData(vertices, indices);

    auto& ComponentMesh = m_Scene->registry.emplace<COMPMesh>(entity, mesh);

    Transform.SetPosition({0.f, 0.f, 0.f});

    // 2nd triangle
    auto entity_b = m_Scene->registry.create();
    auto& Geometry_b = m_Scene->registry.emplace<COMPGeometry>(entity_b, vertices, indices);
    auto& Transform_b = m_Scene->registry.emplace<COMPTransform>(entity_b);
    auto& Material_b = m_Scene->registry.emplace<COMPMaterial>(entity_b, std::move(shader_b), glm::vec3(1.f, 0.f, 0.f));
    m_Scene->registry.emplace<TAG_GameLayer>(entity_b);

    auto mesh_b = std::make_shared<Mesh>();
    mesh_b->SetData(vertices, indices);

    auto& ComponentMesh_b = m_Scene->registry.emplace<COMPMesh>(entity_b, mesh_b);

    Transform_b.SetPosition({1.f, 1.f, 0.f});

    // Camera creation
    auto context_expected = m_Scene->GetContext();
    if (context_expected.has_value())
    {
        EngineContext* context = context_expected.value();

        auto cam_entity = m_Scene->registry.create();
        auto& cam_transform = m_Scene->registry.emplace<COMPTransform>(cam_entity);
        cam_transform.SetPosition({0.f, 0.f, -2.f});
        
        float ratio = static_cast<float>(context->WindowWidth) / static_cast<float>(context->WindowHeight);
        
        m_Scene->registry.emplace<COMPCamera>(cam_entity, 90.f, ratio, 0.01f, 1000.f);
        m_Scene->m_CameraManager.SetActiveCamera(cam_entity);
    }else {
        logger.AppendLogTag("GAMELAYER", LogColors::GREEN);
        logger.LogError("Camera has not been attached, because no context had been provided!");
        logger.DumpLogs();
    }

    
}

void GameLayer::OnDetach()
{
    auto view = m_Scene->registry.view<TAG_GameLayer>();
    view.each([&](auto entity){
        m_Scene->registry.destroy(entity);
    });
}

void GameLayer::OnEvent(Event& e)
{
    if (e.GetType() == WindowResizeEvent::GetStaticType())
    {
        auto& resize = static_cast<WindowResizeEvent&>(e);

        float ratio = static_cast<float>(resize.Width) / static_cast<float>(resize.Height);
        auto view = m_Scene->registry.view<COMPCamera>();
        auto* cam = m_Scene->registry.try_get<COMPCamera>(view.front());
        if (cam)
        {
            cam->SetRatio(ratio);
        }

        std::cout << "Resize: " << resize.Width << ", " << resize.Height << std::endl;
    }

    if (e.GetType() == TestEvent::GetStaticType())
    {
        auto& test = static_cast<TestEvent&>(e);
    }

    e.Handled = true; // Set handled to true, since the layer is the last destination
}