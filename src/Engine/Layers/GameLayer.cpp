#include <Engine/Layers/GameLayer.hpp>

void GameLayer::OnDraw(Scene& scene, entt::registry& registry)
{
    int x = 1;
}

void GameLayer::OnUpdate(entt::registry& registry, float dt)
{
    registry.view<COMPTransform>().each([&](auto entity, COMPTransform& transform){
        transform.Rotate({0.f, 0.f, 10.f * dt});
    });
}

void GameLayer::OnAttach(Scene& scene, entt::registry& registry)
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
    auto entity = registry.create();
    auto& Geometry = registry.emplace<COMPGeometry>(entity, vertices, indices);
    auto& Transform = registry.emplace<COMPTransform>(entity);
    auto& Material = registry.emplace<COMPMaterial>(entity, std::move(shader), glm::vec3(1.f, 0.f, 0.f));
    registry.emplace<TAG_GameLayer>(entity);

    auto mesh = std::make_shared<Mesh>();
    mesh->SetData(vertices, indices);

    auto& ComponentMesh = registry.emplace<COMPMesh>(entity, mesh);

    Transform.SetPosition({0.f, 0.f, 0.f});

    // 2nd triangle
    auto entity_b = registry.create();
    auto& Geometry_b = registry.emplace<COMPGeometry>(entity_b, vertices, indices);
    auto& Transform_b = registry.emplace<COMPTransform>(entity_b);
    auto& Material_b = registry.emplace<COMPMaterial>(entity_b, std::move(shader_b), glm::vec3(1.f, 0.f, 0.f));
    registry.emplace<TAG_GameLayer>(entity_b);

    auto mesh_b = std::make_shared<Mesh>();
    mesh_b->SetData(vertices, indices);

    auto& ComponentMesh_b = registry.emplace<COMPMesh>(entity_b, mesh_b);

    Transform_b.SetPosition({1.f, 1.f, 0.f});

    // Camera
    auto cam_entity = registry.create();
    auto& cam_transform = registry.emplace<COMPTransform>(cam_entity);
    cam_transform.SetPosition({0.f, 0.f, -2.f});

    float ratio = static_cast<float>(scene.m_EngineContext.Width) / static_cast<float>(scene.m_EngineContext.Height);
    
    registry.emplace<COMPCamera>(cam_entity, 90.f, ratio, 0.01f, 1000.f);
    scene.m_CameraManager.SetActiveCamera(cam_entity);
}

void GameLayer::OnDetach(entt::registry& registry)
{
    auto view = registry.view<TAG_GameLayer>();
    view.each([&](auto entity){
        registry.destroy(entity);
    });
}

void GameLayer::OnEvent(entt::registry& registry, Event& e)
{
    if (auto *resizeEvent = dynamic_cast<WindowResizeEvent*>(&e))
    {
        float ratio = static_cast<float>(resizeEvent->Width) / static_cast<float>(resizeEvent->Height);
        auto view = registry.view<COMPCamera>();
        auto* cam = registry.try_get<COMPCamera>(view.front());
        if (cam)
        {
            cam->SetRatio(ratio);
        }
    }
}