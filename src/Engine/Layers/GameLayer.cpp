#include <Engine/Layers/GameLayer.hpp>

#include "App/Services.hpp"

/**
 * @brief Executes the layer's per-frame drawing step.
 *
 * This is invoked during the render phase to perform any layer-specific rendering.
 * Currently no rendering operations are implemented in this method.
 */
void GameLayer::OnDraw()
{
    int x = 1;
}

/**
 * @brief Updates the active camera's transform based on keyboard input.
 *
 * Reads WASD for movement in the camera's local forward/right plane and Q/E for vertical movement,
 * computes a normalized movement direction if non-zero, and applies it to the active camera transform scaled by a fixed speed and the supplied delta time.
 *
 * @param dt Elapsed time since the last update, in seconds.
 */
void GameLayer::OnUpdate(float dt)
{    
    // Camera movement
    auto active_cam = m_Scene->m_CameraManager.GetActiveCamera();
    auto* transform = m_Scene->registry.try_get<COMPTransform>(active_cam);
    auto* cam = m_Scene->registry.try_get<COMPCamera>(active_cam);

    if (transform && cam)
    {
        float speed = 2.0f;

        glm::vec3 input(0);
        // WASD
        if (InputSystem::IsKeyHeld(GLFW_KEY_W)) input.z += 1;
        if (InputSystem::IsKeyHeld(GLFW_KEY_S)) input.z -= 1;
        if (InputSystem::IsKeyHeld(GLFW_KEY_A)) input.x -= 1;
        if (InputSystem::IsKeyHeld(GLFW_KEY_D)) input.x += 1;
        
        // QE
        if (InputSystem::IsKeyHeld(GLFW_KEY_Q)) input.y -= 1.f;
        if (InputSystem::IsKeyHeld(GLFW_KEY_E)) input.y += 1.f;
        
        glm::vec3 move = input.x * cam->GetRight() +
                         input.y * glm::vec3(0.f, 1.f, 0.f) +
                         input.z * cam->GetForward();
        
        if (glm::length(move) > 0.f)
            move = glm::normalize(move);
        
        transform->Move(move * speed * dt);
    }
}

/**
 * @brief Initialize the layer's scene: load assets, create entities, lights, and an active camera.
 *
 * Loads shaders and textures, creates materials and a shared mesh, and instantiates two quad entities
 * (each with geometry, transform, material, and mesh components) tagged for the GameLayer. Configures
 * a spotlight and, if an EngineContext is available, creates a camera positioned at {0,0,-2} with
 * a 90° field of view and the window aspect ratio, then sets it as the active camera. If no context
 * is present, an error is logged and camera creation is skipped.
 */
void GameLayer::OnAttach()
{
    std::vector<Vertex> vertices = {
        // Front face (normal 0,0,1)
        {{-0.5f, 0.f, 0.f}, {1,0,0,1}, {0.f, 0.f}, {0.f, 0.f, 1.f}},
        {{-0.5f, 0.5f, 0.f}, {0,1,0,1}, {0.f, 1.f}, {0.f, 0.f, 1.f}},
        {{0.5f, 0.f, 0.f}, {0,0,1,1}, {1.f, 0.f}, {0.f, 0.f, 1.f}},
        {{0.5f, 0.5f, 0.f}, {1,1,0,1}, {1.f, 1.f}, {0.f, 0.f, 1.f}},

        // Back face (normal 0,0,-1)
        {{0.5f, 0.f, -0.5f}, {0,1,1,1}, {0.f, 0.f}, {0.f, 0.f, -1.f}},
        {{0.5f, 0.5f, -0.5f}, {1,0,1,1}, {0.f, 1.f}, {0.f, 0.f, -1.f}},
        {{-0.5f, 0.f, -0.5f}, {1,1,1,1}, {1.f, 0.f}, {0.f, 0.f, -1.f}},
        {{-0.5f, 0.5f, -0.5f}, {0,0,0,1}, {1.f, 1.f}, {0.f, 0.f, -1.f}},

        // Left face (normal -1,0,0)
        {{-0.5f, 0.f, -0.5f}, {1,0,0,1}, {0.f, 0.f}, {-1.f, 0.f, 0.f}},
        {{-0.5f, 0.5f, -0.5f}, {0,1,0,1}, {0.f, 1.f}, {-1.f, 0.f, 0.f}},
        {{-0.5f, 0.f, 0.f}, {0,0,1,1}, {1.f, 0.f}, {-1.f, 0.f, 0.f}},
        {{-0.5f, 0.5f, 0.f}, {1,1,0,1}, {1.f, 1.f}, {-1.f, 0.f, 0.f}},

        // Right face (normal 1,0,0)
        {{0.5f, 0.f, 0.f}, {0,1,1,1}, {0.f, 0.f}, {1.f, 0.f, 0.f}},
        {{0.5f, 0.5f, 0.f}, {1,0,1,1}, {0.f, 1.f}, {1.f, 0.f, 0.f}},
        {{0.5f, 0.f, -0.5f}, {1,1,1,1}, {1.f, 0.f}, {1.f, 0.f, 0.f}},
        {{0.5f, 0.5f, -0.5f}, {0,0,0,1}, {1.f, 1.f}, {1.f, 0.f, 0.f}},

        // Top face (normal 0,1,0)
        {{-0.5f, 0.5f, 0.f}, {1,0,0,1}, {0.f, 0.f}, {0.f, 1.f, 0.f}},
        {{-0.5f, 0.5f, -0.5f}, {0,1,0,1}, {0.f, 1.f}, {0.f, 1.f, 0.f}},
        {{0.5f, 0.5f, 0.f}, {0,0,1,1}, {1.f, 0.f}, {0.f, 1.f, 0.f}},
        {{0.5f, 0.5f, -0.5f}, {1,1,0,1}, {1.f, 1.f}, {0.f, 1.f, 0.f}},

        // Bottom face (normal 0,-1,0)
        {{-0.5f, 0.f, -0.5f}, {0,1,1,1}, {0.f, 0.f}, {0.f, -1.f, 0.f}},
        {{-0.5f, 0.f, 0.f}, {1,0,1,1}, {0.f, 1.f}, {0.f, -1.f, 0.f}},
        {{0.5f, 0.f, -0.5f}, {1,1,1,1}, {1.f, 0.f}, {0.f, -1.f, 0.f}},
        {{0.5f, 0.f, 0.f}, {0,0,0,1}, {1.f, 1.f}, {0.f, -1.f, 0.f}},
    };

    std::vector<GLuint> indices;
    for (int i = 0; i < 6; i++) {
        unsigned int offset = i * 4;
        indices.push_back(offset + 0);
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);
        indices.push_back(offset + 2);
        indices.push_back(offset + 1);
        indices.push_back(offset + 3);
    }

    for (auto &v : vertices) {
        v.color = {1.f, 1.f, 1.f, 1.f};
    }

    auto context_result = m_Scene->GetContext();
    if (!context_result.has_value())
    {
        logger.AppendLogTag("GAMELAYER", LogColors::GREEN);
        logger.LogError("Cannot initialize: missing context!");
        logger.DumpLogs();
        return;
    }
    auto&services = Services::Get();

    auto& shader_manager = services.GetService<ShaderManager>();
    auto& material_manager = services.GetService<MaterialManager>();
    auto& texture_manager = services.GetService<Texture2DManager>();
    auto& light_manager = m_Scene->m_LightManager;
    // shaders
    auto shader = shader_manager.Load("../Shaders/first.vert", "../Shaders/first.frag");
    auto sunShader = shader_manager.Load("../Shaders/first.vert", "../Shaders/sun.frag");

    // textures
    auto basic_texture = texture_manager.Load("Resources/Textures2D/images.png");
    auto color_grid_texture = texture_manager.Load("Resources/Textures2D/color_grid.png");

    // Materials
    uint32_t MaterialID = material_manager.CreateMaterial(shader, basic_texture.value());
    uint32_t MaterialID_colorgrid = material_manager.CreateMaterial(sunShader, color_grid_texture.value());

    // first quad
    auto quad_1 = m_Scene->registry.create();
    auto& Geometry = m_Scene->registry.emplace<COMPGeometry>(quad_1, vertices, indices);
    auto& Transform = m_Scene->registry.emplace<COMPTransform>(quad_1);
    auto& Material = m_Scene->registry.emplace<COMPMaterial>(quad_1, MaterialID);

    m_Scene->registry.emplace<TAG_GameLayer>(quad_1);
    std::vector<COMPTexture> textures;
    textures.push_back(COMPTexture{basic_texture.value()});

    auto mesh = std::make_shared<Mesh>(vertices, indices);
    mesh->SetData();

    auto& ComponentMesh = m_Scene->registry.emplace<COMPMesh>(quad_1, mesh);


    Transform.SetPosition({0.f, 0.f, 0.f});

    // second quad
    auto quad_2 = m_Scene->registry.create();
    auto& Geometry_a = m_Scene->registry.emplace<COMPGeometry>(quad_2, vertices, indices);
    auto& Transform_a = m_Scene->registry.emplace<COMPTransform>(quad_2);
    auto& Material_a = m_Scene->registry.emplace<COMPMaterial>(quad_2, MaterialID);
    m_Scene->registry.emplace<TAG_GameLayer>(quad_2);

    // TODO: Let meshes be reusable
    m_Scene->registry.emplace<COMPMesh>(quad_2, mesh);

    Transform_a.SetPosition({3.f, 1.f, -2.f});

    // THE SUNS
    // i dont need many if checks, fairly confident this'll work
    auto lightID = light_manager->CreateLight<SpotLight>();
    auto light = light_manager->GetLight<SpotLight>(lightID.value());
    light->cutOff = 80.f;
    light->outerCutOff = 120.f;

    glm::vec3 target = {0.f, 0.f, 0.f};
    light->position = {2.f, 0.f, 0.f};
    light->direction = glm::normalize(target - light->position);

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

/**
 * @brief Handle window, keyboard, and mouse events for the game layer, updating cameras, transforms, and input state.
 *
 * Processes:
 * - WindowResizeEvent: recomputes and applies the new aspect ratio to the active camera.
 * - KeyInputEvent: toggles cursor/raw-mouse mode on Escape, adjusts an internal placement distance with C/Z, and on F constructs and positions a new geometry entity relative to the active camera.
 * - MouseMoveEvent: when mouse control is enabled and an active camera exists, applies yaw/pitch rotation to the active camera from mouse deltas.
 *
 * The function marks the incoming event as handled (e.Handled = true) when complete.
 *
 * @param e The event to process; this function may modify e.Handled.
 */
void GameLayer::OnEvent(Event& e)
{

    if (e.GetType() == WindowResizeEvent::GetStaticType())
    {
        const auto& resize = static_cast<WindowResizeEvent&>(e);

        const float ratio = static_cast<float>(resize.Width) / static_cast<float>(resize.Height);
        auto view = m_Scene->registry.view<COMPCamera>();
        if (auto* cam = m_Scene->registry.try_get<COMPCamera>(view.front()))
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
                m_CanMoveMouse = true;
            }else {
                clicked = false;
                glfwSetInputMode(input.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetInputMode(input.Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
                m_CanMoveMouse = false;
            }
        }
        

        static float distance = 2.f;

        if (input.IsKey(GLFW_KEY_C) && input.IsKeyPressed())
        {
            distance += 0.5f;

        }
        if (input.IsKey(GLFW_KEY_Z) && input.IsKeyPressed())
        {
            distance -= 0.5f;
        }    
        if (input.IsKey(GLFW_KEY_F) && input.IsKeyPressed())
        {
            auto cam = m_Scene->m_CameraManager.GetActiveCamera();
            auto& camTransform = m_Scene->registry.get<COMPTransform>(cam);
            auto& camComponent = m_Scene->registry.get<COMPCamera>(cam);

             std::vector<Vertex> vertices = {
                // Front face (normal 0,0,1)
                {{-0.5f, 0.f, 0.f}, {1,0,0,1}, {0.f, 0.f}, {0.f, 0.f, 1.f}},
                {{-0.5f, 0.5f, 0.f}, {0,1,0,1}, {0.f, 1.f}, {0.f, 0.f, 1.f}},
                {{0.5f, 0.f, 0.f}, {0,0,1,1}, {1.f, 0.f}, {0.f, 0.f, 1.f}},
                {{0.5f, 0.5f, 0.f}, {1,1,0,1}, {1.f, 1.f}, {0.f, 0.f, 1.f}},

                // Back face (normal 0,0,-1)
                {{0.5f, 0.f, -0.5f}, {0,1,1,1}, {0.f, 0.f}, {0.f, 0.f, -1.f}},
                {{0.5f, 0.5f, -0.5f}, {1,0,1,1}, {0.f, 1.f}, {0.f, 0.f, -1.f}},
                {{-0.5f, 0.f, -0.5f}, {1,1,1,1}, {1.f, 0.f}, {0.f, 0.f, -1.f}},
                {{-0.5f, 0.5f, -0.5f}, {0,0,0,1}, {1.f, 1.f}, {0.f, 0.f, -1.f}},

                // Left face (normal -1,0,0)
                {{-0.5f, 0.f, -0.5f}, {1,0,0,1}, {0.f, 0.f}, {-1.f, 0.f, 0.f}},
                {{-0.5f, 0.5f, -0.5f}, {0,1,0,1}, {0.f, 1.f}, {-1.f, 0.f, 0.f}},
                {{-0.5f, 0.f, 0.f}, {0,0,1,1}, {1.f, 0.f}, {-1.f, 0.f, 0.f}},
                {{-0.5f, 0.5f, 0.f}, {1,1,0,1}, {1.f, 1.f}, {-1.f, 0.f, 0.f}},

                // Right face (normal 1,0,0)
                {{0.5f, 0.f, 0.f}, {0,1,1,1}, {0.f, 0.f}, {1.f, 0.f, 0.f}},
                {{0.5f, 0.5f, 0.f}, {1,0,1,1}, {0.f, 1.f}, {1.f, 0.f, 0.f}},
                {{0.5f, 0.f, -0.5f}, {1,1,1,1}, {1.f, 0.f}, {1.f, 0.f, 0.f}},
                {{0.5f, 0.5f, -0.5f}, {0,0,0,1}, {1.f, 1.f}, {1.f, 0.f, 0.f}},

                // Top face (normal 0,1,0)
                {{-0.5f, 0.5f, 0.f}, {1,0,0,1}, {0.f, 0.f}, {0.f, 1.f, 0.f}},
                {{-0.5f, 0.5f, -0.5f}, {0,1,0,1}, {0.f, 1.f}, {0.f, 1.f, 0.f}},
                {{0.5f, 0.5f, 0.f}, {0,0,1,1}, {1.f, 0.f}, {0.f, 1.f, 0.f}},
                {{0.5f, 0.5f, -0.5f}, {1,1,0,1}, {1.f, 1.f}, {0.f, 1.f, 0.f}},

                // Bottom face (normal 0,-1,0)
                {{-0.5f, 0.f, -0.5f}, {0,1,1,1}, {0.f, 0.f}, {0.f, -1.f, 0.f}},
                {{-0.5f, 0.f, 0.f}, {1,0,1,1}, {0.f, 1.f}, {0.f, -1.f, 0.f}},
                {{0.5f, 0.f, -0.5f}, {1,1,1,1}, {1.f, 0.f}, {0.f, -1.f, 0.f}},
                {{0.5f, 0.f, 0.f}, {0,0,0,1}, {1.f, 1.f}, {0.f, -1.f, 0.f}},
            };

            for (auto &vertex : vertices) {
                vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
            }

            std::vector<GLuint> indices;
            for (int i = 0; i < 6; i++) {
                unsigned int offset = i * 4;
                indices.push_back(offset + 0);
                indices.push_back(offset + 1);
                indices.push_back(offset + 2);
                indices.push_back(offset + 2);
                indices.push_back(offset + 1);
                indices.push_back(offset + 3);
            }

            auto& renderer = Services::Get().GetService<Renderer>();
            auto& shader_manager = Services::Get().GetService<ShaderManager>();
            auto& material_manager = Services::Get().GetService<MaterialManager>();
            auto& texture_manager = Services::Get().GetService<Texture2DManager>();
            
            // shaders
            auto shader = shader_manager.Load("../Shaders/first.vert", "../Shaders/first.frag");

            // textures
            auto basic_texture = texture_manager.Load("Resources/Textures2D/images.png");

            // Materials
            uint32_t MaterialID = 0;
            // first quad
            auto quad_1 = m_Scene->registry.create();
            auto& Geometry = m_Scene->registry.emplace<COMPGeometry>(quad_1, vertices, indices);
            auto& Transform = m_Scene->registry.emplace<COMPTransform>(quad_1);
            auto& Material = m_Scene->registry.emplace<COMPMaterial>(quad_1, MaterialID);
            
            m_Scene->registry.emplace<TAG_GameLayer>(quad_1);

            auto view = m_Scene->registry.view<COMPMesh>();
            auto& otherMesh = m_Scene->registry.get<COMPMesh>(view.front());

            auto& componentMesh = m_Scene->registry.emplace<COMPMesh>(quad_1, otherMesh.mesh);

            Transform.SetPosition(camTransform.position + camComponent.GetForward() * distance);
        }
    }

    if (e.GetType() == MouseMoveEvent::GetStaticType())
    {
        if (!m_CanMoveMouse) return;
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