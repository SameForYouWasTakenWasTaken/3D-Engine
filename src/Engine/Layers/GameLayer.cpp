#include <Engine/Layers/GameLayer.hpp>

void GameLayer::OnDraw()
{
    int x = 1;
}

void GameLayer::OnUpdate(float dt)
{    
    auto view = m_Scene->registry.view<COMPGeometry>();
    // Camera movement
    auto active_cam = m_Scene->m_CameraManager.GetActiveCamera();
    auto* transform = m_Scene->registry.try_get<COMPTransform>(active_cam);
    auto* cam = m_Scene->registry.try_get<COMPCamera>(active_cam);

    if (transform && cam)
    {
        float speed = 2.0f;
        float rotate_speed = 50.f;

        glm::vec3 input(0);
        // WASD
        if (InputSystem::IsKeyHeld(GLFW_KEY_W)) input.z += 1;
        if (InputSystem::IsKeyHeld(GLFW_KEY_S)) input.z -= 1;
        if (InputSystem::IsKeyHeld(GLFW_KEY_A)) input.x -= 1;
        if (InputSystem::IsKeyHeld(GLFW_KEY_D)) input.x += 1;
        
        // QE
        if (InputSystem::IsKeyHeld(GLFW_KEY_Q)) input.y -= 1.f;
        if (InputSystem::IsKeyHeld(GLFW_KEY_E)) input.y += 1.f;

        // Look around with arrow keys
        if (InputSystem::IsKeyHeld(GLFW_KEY_UP)) cam->RotateEuler(0, rotate_speed * dt);
        if (InputSystem::IsKeyHeld(GLFW_KEY_DOWN)) cam->RotateEuler(0, -rotate_speed * dt);
        if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT)) cam->RotateEuler(-rotate_speed * dt, 0);
        if (InputSystem::IsKeyHeld(GLFW_KEY_RIGHT)) cam->RotateEuler(rotate_speed * dt, 0);
        
        glm::vec3 move = input.x * cam->GetRight() +
                         input.y * glm::vec3(0.f, 1.f, 0.f) +
                         input.z * cam->GetForward();
        
        if (glm::length(move) > 0.f)
            move = glm::normalize(move);
        
        transform->Move(move * speed * dt);
    }
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
    std::unique_ptr<Shader> shader_c = std::make_unique<Shader>("Shaders/first.vert", "Shaders/first.frag");


    // first triangle
    auto triangle_1 = m_Scene->registry.create();
    auto& Geometry = m_Scene->registry.emplace<COMPGeometry>(triangle_1, vertices, indices);
    auto& Transform = m_Scene->registry.emplace<COMPTransform>(triangle_1);
    auto& Material = m_Scene->registry.emplace<COMPMaterial>(triangle_1, std::move(shader), glm::vec3(1.f, 0.f, 0.f));
    m_Scene->registry.emplace<TAG_GameLayer>(triangle_1);

    auto mesh = std::make_shared<Mesh>();
    mesh->SetData(vertices, indices);

    auto& ComponentMesh = m_Scene->registry.emplace<COMPMesh>(triangle_1, mesh);

    Transform.SetPosition({0.f, 0.f, 0.f});

    // 2nd triangle
    auto triangle_2 = m_Scene->registry.create();
    auto& Geometry_b = m_Scene->registry.emplace<COMPGeometry>(triangle_2, vertices, indices);
    auto& Transform_b = m_Scene->registry.emplace<COMPTransform>(triangle_2);
    auto& Material_b = m_Scene->registry.emplace<COMPMaterial>(triangle_2, std::move(shader_b), glm::vec3(1.f, 0.f, 0.f));
    m_Scene->registry.emplace<TAG_GameLayer>(triangle_2);

    auto mesh_b = std::make_shared<Mesh>();
    mesh_b->SetData(vertices, indices);

    auto& ComponentMesh_b = m_Scene->registry.emplace<COMPMesh>(triangle_2, mesh_b);

    Transform_b.SetPosition({1.f, 1.f, 0.f});

    // circle
    std::vector<Vertex> special_vertices = {};
    std::vector<GLuint> special_indices = {};
    glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
    special_vertices.push_back({ {0.f, 0.f, 0.f}, color }); // center
  
    size_t iter_num = 100;
    float radius = 2.f;

	for (size_t i = 0; i <= iter_num; i++) {
		float theta = (2 * 3.141592653589 * i) / iter_num;
		float x = radius * cos(theta);
		float y = radius * sin(theta);
		special_vertices.push_back({ {x, y, 0}, color });
	}

    auto circle = m_Scene->registry.create();
    auto& geometry_c = m_Scene->registry.emplace<COMPGeometry>(circle, special_vertices, special_indices);
    auto& Transform_c = m_Scene->registry.emplace<COMPTransform>(circle);
    auto& Material_c = m_Scene->registry.emplace<COMPMaterial>(circle, std::move(shader_c), glm::vec3(1.f, 0.f, 0.f));
    m_Scene->registry.emplace<TAG_GameLayer>(circle);

    auto mesh_c = std::make_shared<Mesh>();
    mesh_c->SetData(special_vertices, special_indices);
    mesh_c->Primitive = GL_TRIANGLE_FAN;
    mesh_c->Indexed = false;

    auto& ComponentMesh_c = m_Scene->registry.emplace<COMPMesh>(circle, mesh_c);
    Transform_c.SetPosition({2.f, 2.f, 1.f});

    for (int i = 0; i < 50; i++)
    {
        int x = 10;
    }

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

    if (e.GetType() == KeyInputEvent::GetStaticType())
    {
        auto& input = static_cast<KeyInputEvent&>(e);
        
        static bool clicked = false;
        if (input.IsKey(GLFW_KEY_ESCAPE) && input.IsKeyPressed())
        {
            if (!clicked)
            {
                clicked = true;
                glfwSetInputMode(input.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetInputMode(input.Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); 
            }else {
                clicked = false;
                glfwSetInputMode(input.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetInputMode(input.Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            }
        }
        if (input.IsKey(GLFW_KEY_U) && input.IsKeyPressed())
        {
            auto view = m_Scene->registry.view<COMPTransform, COMPGeometry>();
            view.each([&](auto entity, COMPTransform& transform, COMPGeometry& geometry){
                transform.Rotate({0.f, 0.f, 30.f});
                transform.Move({.5f, 0.f, 0.f});
                transform.Scale({0.5f, 0.5f, 0.5f});
            });
        }
        if (input.IsKey(GLFW_KEY_I) && input.IsKeyPressed())
        {
            auto view = m_Scene->registry.view<COMPTransform, COMPGeometry>();
            view.each([&](auto entity, COMPTransform& transform, COMPGeometry& geometry){
                transform.Rotate({0.f, 0.f, -30.f});
                transform.Move({-.5f, 0.f, 0.f});
                transform.Scale({-0.5f, -0.5f, -0.5f});
            });
        }
    }

    if (e.GetType() == MouseMoveEvent::GetStaticType())
    {
        if (m_Scene->m_CameraManager.GetActiveCamera() == entt::null) return;

        auto& mouse = static_cast<MouseMoveEvent&>(e);
        auto& cam = m_Scene->registry.get<COMPCamera>(m_Scene->m_CameraManager.GetActiveCamera());
        
        static constexpr float sensitivity = 0.1f;
        static bool firstMouseInput = true;
        static double lastMouseX;
        static double lastMouseY;

        if (firstMouseInput) {
            lastMouseX = mouse.xpos;
            lastMouseY = mouse.ypos;
            firstMouseInput = false; // reset after first alignment
            return;
        }

        float xoffset = mouse.xpos - lastMouseX;
        float yoffset = lastMouseY - mouse.ypos; // reversed since y-coords go top->down

        lastMouseX = mouse.xpos;
        lastMouseY = mouse.ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        cam.RotateEuler(xoffset, yoffset);
    }

    e.Handled = true; // Set handled to true, since the layer is the last destination
}