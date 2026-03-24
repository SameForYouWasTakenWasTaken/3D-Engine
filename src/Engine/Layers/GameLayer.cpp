#include <Engine/Layers/GameLayer.hpp>

#include "App/Services.hpp"
#include <Components/Model.hpp>

#include "Engine/Systems/AssetManager.hpp"

#include <Engine/Systems/HierarchySystem.hpp>

#include "App/App.hpp"

std::pair<std::vector<Vertex>, std::vector<GLuint>> GetDefault()
{
    std::vector<Vertex> vertices = {
        // Front face (normal 0,0,1)
        {{-0.5f, 0.f, 0.f}, {1, 0, 0, 1}, {0.f, 0.f}, {0.f, 0.f, 1.f}},
        {{-0.5f, 0.5f, 0.f}, {0, 1, 0, 1}, {0.f, 1.f}, {0.f, 0.f, 1.f}},
        {{0.5f, 0.f, 0.f}, {0, 0, 1, 1}, {1.f, 0.f}, {0.f, 0.f, 1.f}},
        {{0.5f, 0.5f, 0.f}, {1, 1, 0, 1}, {1.f, 1.f}, {0.f, 0.f, 1.f}},

        // Back face (normal 0,0,-1)
        {{0.5f, 0.f, -0.5f}, {0, 1, 1, 1}, {0.f, 0.f}, {0.f, 0.f, -1.f}},
        {{0.5f, 0.5f, -0.5f}, {1, 0, 1, 1}, {0.f, 1.f}, {0.f, 0.f, -1.f}},
        {{-0.5f, 0.f, -0.5f}, {1, 1, 1, 1}, {1.f, 0.f}, {0.f, 0.f, -1.f}},
        {{-0.5f, 0.5f, -0.5f}, {0, 0, 0, 1}, {1.f, 1.f}, {0.f, 0.f, -1.f}},

        // Left face (normal -1,0,0)
        {{-0.5f, 0.f, -0.5f}, {1, 0, 0, 1}, {0.f, 0.f}, {-1.f, 0.f, 0.f}},
        {{-0.5f, 0.5f, -0.5f}, {0, 1, 0, 1}, {0.f, 1.f}, {-1.f, 0.f, 0.f}},
        {{-0.5f, 0.f, 0.f}, {0, 0, 1, 1}, {1.f, 0.f}, {-1.f, 0.f, 0.f}},
        {{-0.5f, 0.5f, 0.f}, {1, 1, 0, 1}, {1.f, 1.f}, {-1.f, 0.f, 0.f}},

        // Right face (normal 1,0,0)
        {{0.5f, 0.f, 0.f}, {0, 1, 1, 1}, {0.f, 0.f}, {1.f, 0.f, 0.f}},
        {{0.5f, 0.5f, 0.f}, {1, 0, 1, 1}, {0.f, 1.f}, {1.f, 0.f, 0.f}},
        {{0.5f, 0.f, -0.5f}, {1, 1, 1, 1}, {1.f, 0.f}, {1.f, 0.f, 0.f}},
        {{0.5f, 0.5f, -0.5f}, {0, 0, 0, 1}, {1.f, 1.f}, {1.f, 0.f, 0.f}},

        // Top face (normal 0,1,0)
        {{-0.5f, 0.5f, 0.f}, {1, 0, 0, 1}, {0.f, 0.f}, {0.f, 1.f, 0.f}},
        {{-0.5f, 0.5f, -0.5f}, {0, 1, 0, 1}, {0.f, 1.f}, {0.f, 1.f, 0.f}},
        {{0.5f, 0.5f, 0.f}, {0, 0, 1, 1}, {1.f, 0.f}, {0.f, 1.f, 0.f}},
        {{0.5f, 0.5f, -0.5f}, {1, 1, 0, 1}, {1.f, 1.f}, {0.f, 1.f, 0.f}},

        // Bottom face (normal 0,-1,0)
        {{-0.5f, 0.f, -0.5f}, {0, 1, 1, 1}, {0.f, 0.f}, {0.f, -1.f, 0.f}},
        {{-0.5f, 0.f, 0.f}, {1, 0, 1, 1}, {0.f, 1.f}, {0.f, -1.f, 0.f}},
        {{0.5f, 0.f, -0.5f}, {1, 1, 1, 1}, {1.f, 0.f}, {0.f, -1.f, 0.f}},
        {{0.5f, 0.f, 0.f}, {0, 0, 0, 1}, {1.f, 1.f}, {0.f, -1.f, 0.f}},
    };

    std::vector<GLuint> indices;
    for (int i = 0; i < 6; i++)
    {
        unsigned int offset = i * 4;
        indices.push_back(offset + 0);
        indices.push_back(offset + 2);
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);
        indices.push_back(offset + 3);
        indices.push_back(offset + 1);
    }

    for (auto& v : vertices)
    {
        v.color = {1.f, 1.f, 1.f, 1.f};
    }

    return {vertices, indices};
}

/**
 * @brief Perform per-frame render submission for the current scene.
 *
 * Begins a rendering frame, validates the active camera, and submits visible geometry and model submeshes
 * to the renderer for the active scene. If no active camera transform or camera component is available,
 * the function logs a warning and skips submission for this frame.
 *
 * The method:
 * - Obtains renderer and asset manager services.
 * - Submits entities with COMPGeometry/COMPMesh/COMPMaterial/COMPTransform as single drawables.
 * - Submits entities with COMPModel/COMPTransform by loading the model, iterating its submeshes and submitting
 *   each submesh. Per-entity MaterialOverride components are applied via SetMaterialOverrides when present.
 */
void GameLayer::OnDraw()
{
    auto& renderer = Services::Get().GetService<Renderer>();
    auto& assetManager = Services::Get().GetService<AssetManager>();

    auto* cam = m_Scene->registry.try_get<COMPCamera>(m_Scene->m_CameraManager.GetActiveCamera());
    auto* camTransform = m_Scene->registry.try_get<COMPTransform>(m_Scene->m_CameraManager.GetActiveCamera());


    if (cam == nullptr || camTransform == nullptr)
    {
        logger.AppendLogTag("GAMELAYER", LogColors::GREEN);
        logger.AppendLogTag("CAMERA_MANAGER", LogColors::YELLOW);
        logger.LogWarning("No active camera set, skipping draw.");
        return;
    }
    auto& context = renderer.m_EngineContext;

    auto generalView = m_Scene->registry.view<
        COMPMesh,
        COMPMaterial,
        COMPTransform
    >();

    auto modelView = m_Scene->registry.view<COMPModel, COMPTransform>();

    generalView.each([&](auto entity,
                         COMPMesh& mesh,
                         COMPMaterial& material,
                         COMPTransform& transform)
    {
        SubmitObject submit{};
        submit.mesh = mesh.mesh.get();
        submit.active_scene = m_Scene;
        submit.materialID = material.material;
        submit.transform = &transform;
        // Put everything into the renderer
        renderer.Submit(
            submit
        );
    });


    modelView.each([&](auto entity, COMPModel& compModel, COMPTransform& transform)
    {
        auto* model = assetManager.Get(compModel.id);
        if (model == nullptr)
            return;

        auto subMeshes = model->GetSubMeshes();

        // Set optional overrides
        auto* MatOverride = m_Scene->registry.try_get<MaterialOverride>(entity);
        if (MatOverride)
            SetMaterialOverrides(compModel, *MatOverride);

        int countMesh = 0;
        for (const auto& subMesh : subMeshes)
        {
            MaterialID materialID = subMesh.material;

            // Set mandatory data
            SubmitObject submit{};
            submit.mesh = subMesh.mesh.get();
            submit.active_scene = m_Scene;
            submit.materialID = materialID;
            submit.transform = &transform;

            // Set material overrides
            if (countMesh < compModel.materialOverrides.size())
                submit.materialID = compModel.materialOverrides[countMesh];


            renderer.Submit(submit);
            countMesh++;
        }
    });
}

/**
 * @brief Populate a model component's per-submesh material overrides by cloning and reloading materials with optional property changes.
 *
 * Applies the optional transparency and shininess values from the provided MaterialOverride to each submesh's material,
 * loading a cloned material for each modified submesh and storing the resulting material IDs in modelComponent.materialOverrides.
 * If neither override is specified or the referenced model cannot be found, the function returns without modifying the component.
 * When a submesh's original material cannot be retrieved, the original material ID is preserved in the overrides.
 *
 * @param modelComponent The model component whose `materialOverrides` vector will be replaced to match the model's submesh count.
 * @param materialOverride Optional properties (transparency and/or shininess) to apply to each submesh material.
 */
void GameLayer::SetMaterialOverrides(COMPModel& modelComponent, const MaterialOverride& materialOverride)
{
    bool hasTransparency = materialOverride.transparency.has_value();
    bool hasShininess = materialOverride.shininess.has_value();

    if (!hasTransparency && !hasShininess)
        return;

    auto& services = Services::Get();
    auto& materialManager = services.GetService<MaterialManager>();
    auto& assetManager = services.GetService<AssetManager>();

    Model* model = assetManager.Get(modelComponent.id);
    if (model == nullptr)
        return;

    auto& submeshes = model->GetSubMeshes();

    modelComponent.materialOverrides.clear();
    modelComponent.materialOverrides.reserve(submeshes.size());

    for (auto& subMesh : submeshes)
    {
        Material* originMat = materialManager.Get(subMesh.material);

        if (originMat == nullptr)
        {
            modelComponent.materialOverrides.push_back(subMesh.material);
            continue;
        }

        Material cloneMat = *originMat; // Create a clone


        // modify the clone
        if (hasTransparency)
            cloneMat.transparency = materialOverride.transparency.value();

        if (hasShininess)
            cloneMat.shininess = materialOverride.shininess.value();

        // cache clone to replace material with
        auto matID = materialManager.Load(cloneMat);
        modelComponent.materialOverrides.push_back(matID);
    }
}

void GameLayer::UpdateWorld()
{
    for (const auto& [entity, node] : m_WorldHierarchy.nodes)
    {
        if (node.parent == entt::null)
        {
            UpdateTransforms(entity, glm::mat4(1.0f));
        }
    }
}

void GameLayer::UpdateTransforms(entt::entity entity, const glm::mat4& inheritedWorld)
{
    auto& registry = m_Scene->registry;

    glm::mat4 currentWorld = inheritedWorld;

    if (auto* transform = registry.try_get<COMPTransform>(entity))
    {
        currentWorld = inheritedWorld * transform->GetModelMatrix();
        transform->WorldMatrix = currentWorld;
    }

    const HierarchyNode* node = HierarchySystem::FindInHierarchy(m_WorldHierarchy, entity);
    if (!node)
        return;

    for (entt::entity child : node->children)
    {
        UpdateTransforms(child, currentWorld);
    }
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
    UpdateWorld();
    // Camera movement
    auto active_cam = m_Scene->m_CameraManager.GetActiveCamera();
    auto* transform = m_Scene->registry.try_get<COMPTransform>(active_cam);
    auto* cam = m_Scene->registry.try_get<COMPCamera>(active_cam);

    if (transform && cam)
    {
        float speed = 2.0f;
        float zoomSpeed = 100.f;

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

        if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_SHIFT))
        {
            move *= zoomSpeed;
        }

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

    auto [vertices, indices] = GetDefault();
    auto context_result = m_Scene->GetContext();
    if (!context_result.has_value())
    {
        logger.AppendLogTag("GAMELAYER", LogColors::GREEN);
        logger.LogError("Cannot initialize: missing context!");
        logger.DumpLogs();
        return;
    }
    auto& services = Services::Get();

    auto& shader_manager = services.GetService<ShaderManager>();
    auto& material_manager = services.GetService<MaterialManager>();
    auto& texture_manager = services.GetService<Texture2DManager>();
    auto& asset_manager = services.GetService<AssetManager>();
    auto& light_manager = m_Scene->m_LightManager;
    // shaders
    auto shader = shader_manager.Load("../Shaders/first.vert", "../Shaders/first.frag");
    auto sunShader = shader_manager.Load("../Shaders/first.vert", "../Shaders/sun.frag");

    // textures
    auto basic_texture = texture_manager.Load("Resources/Textures2D/images.png");
    auto color_grid_texture = texture_manager.Load("Resources/Textures2D/default.png");

    // Materials
    auto mat = Material{shader, basic_texture.value()};
    auto sunMat = Material{sunShader, color_grid_texture.value()};

    MaterialID matID = material_manager.Load(mat);
    MaterialID MaterialID_colorgrid = material_manager.Load(sunMat);

    // first quad
    auto quad_1 = m_Scene->registry.create();
    auto& Geometry = m_Scene->registry.emplace<COMPGeometry>(quad_1, vertices, indices);
    auto& Transform = m_Scene->registry.emplace<COMPTransform>(quad_1);
    auto& Material = m_Scene->registry.emplace<COMPMaterial>(quad_1, matID);

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
    auto& Material_a = m_Scene->registry.emplace<COMPMaterial>(quad_2, matID);
    m_Scene->registry.emplace<TAG_GameLayer>(quad_2);

    // TODO: Let meshes be reusable
    m_Scene->registry.emplace<COMPMesh>(quad_2, mesh);

    Transform_a.SetPosition({3.f, 1.f, -2.f});


    // MODELS
    auto defModelID = asset_manager.Load("Resources/Objects/Spiderman/scene.gltf");

    auto ironman = m_Scene->registry.create();

    auto& Transf = m_Scene->registry.emplace<COMPTransform>(ironman);
    auto& model = m_Scene->registry.emplace<COMPModel>(ironman, defModelID);
    Transf.Scale({0.5, 0.5f, 0.5f});

    // THE SUNS
    // i dont need many if checks, fairly confident this'll work
    // Light object
    auto LightEntity = m_Scene->registry.create();
    auto& lightTransform = m_Scene->registry.emplace<COMPTransform>(LightEntity);
    auto& lightMaterial = m_Scene->registry.emplace<COMPMaterial>(LightEntity, matID);

    m_Scene->registry.emplace<TAG_GameLayer>(LightEntity);

    auto lightMesh = std::make_shared<Mesh>(vertices, indices);
    lightMesh->SetData();
    m_Scene->registry.emplace<COMPMesh>(LightEntity, lightMesh);

    auto LightObject = m_Scene->registry.create();
    auto lightID = light_manager->Load<PointLight>();
    auto LightType = light_manager->GetLight<PointLight>(lightID.value());

    LightType->position = {0.f, 0.f, 0.f};

    HierarchySystem::PutInHierarchy(m_WorldHierarchy, quad_1);
    HierarchySystem::PutInHierarchy(m_WorldHierarchy, quad_2);
    HierarchySystem::PutInHierarchy(m_WorldHierarchy, ironman);
    HierarchySystem::PutInHierarchy(m_WorldHierarchy, LightEntity);
    HierarchySystem::SetChildren(m_WorldHierarchy, LightEntity, LightObject);

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
    }
    else
    {
        logger.AppendLogTag("GAMELAYER", LogColors::GREEN);
        logger.LogError("Camera has not been attached, because no context had been provided!");
        logger.DumpLogs();
    }
}

void GameLayer::OnDetach()
{
    auto view = m_Scene->registry.view<TAG_GameLayer>();
    view.each([&](auto entity)
    {
        m_Scene->registry.destroy(entity);
    });
}

/**
 * @brief Process window, keyboard, and mouse events to update camera, transforms, and input state.
 *
 * Handles three event types:
 * - WindowResizeEvent: recomputes the aspect ratio and applies it to the active camera.
 * - KeyInputEvent: toggles cursor/raw-mouse capture on Escape and spawns a model entity with a MaterialOverride when F is pressed.
 * - MouseMoveEvent: when mouse control is enabled and an active camera exists, applies yaw/pitch rotation to the active camera from mouse deltas.
 *
 * The function marks the incoming event as handled (sets `e.Handled = true`) when finished.
 *
 * @param e The event to process; this function may modify `e.Handled`.
 */
void GameLayer::OnEvent(Event& e)
{
    if (e.GetType() == WindowResizeEvent::GetStaticType())
    {
        const auto& resize = dynamic_cast<WindowResizeEvent&>(e);

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
        auto& input = dynamic_cast<KeyInputEvent&>(e);

        static bool clicked = false;
        if (input.IsKey(GLFW_KEY_ESCAPE) && input.IsKeyPressed())
        {
            if (!clicked)
            {
                clicked = true;
                glfwSetInputMode(input.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetInputMode(input.Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                m_CanMoveMouse = true;
            }
            else
            {
                clicked = false;
                glfwSetInputMode(input.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetInputMode(input.Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
                m_CanMoveMouse = false;
            }
        }


        if (input.IsKey(GLFW_KEY_F) && input.IsKeyPressed())
        {
            auto cam = m_Scene->m_CameraManager.GetActiveCamera();
            auto& camTransform = m_Scene->registry.get<COMPTransform>(cam);
            auto& camComponent = m_Scene->registry.get<COMPCamera>(cam);

            auto& services = Services::Get();
            auto& shader_manager = services.GetService<ShaderManager>();
            auto& texture_manager = services.GetService<Texture2DManager>();
            auto& asset_manager = services.GetService<AssetManager>();
            auto& material_manager = services.GetService<MaterialManager>();

            ModelID model_id = asset_manager.Load("Resources/Objects/Spiderman/scene.gltf");
            // model
            auto ModelEntity = m_Scene->registry.create();
            auto& Model = m_Scene->registry.emplace<COMPModel>(ModelEntity, model_id);
            auto& Transform = m_Scene->registry.emplace<COMPTransform>(ModelEntity);
            auto& material = m_Scene->registry.emplace<MaterialOverride>(ModelEntity);
            material.transparency = 4.f;

            m_Scene->registry.emplace<TAG_GameLayer>(ModelEntity);

            Transform.SetPosition(camTransform.LocalPosition + camComponent.GetForward());
            Transform.Scale({0.5f, 0.5f, 0.5f});

            // second quad
            // shaders
            auto shader = shader_manager.Load("../Shaders/first.vert", "../Shaders/first.frag");
            auto sunShader = shader_manager.Load("../Shaders/first.vert", "../Shaders/sun.frag");

            auto basic_texture = texture_manager.Load("Resources/Textures2D/images.png");

            // Materials
            auto mat = Material{shader, basic_texture.value()};

            MaterialID matID = material_manager.Load(mat);

            auto [vertices, indices] = GetDefault();
            auto quad = m_Scene->registry.create();
            auto& Transform_a = m_Scene->registry.emplace<COMPTransform>(quad);
            auto& Material_a = m_Scene->registry.emplace<COMPMaterial>(quad, matID);
            m_Scene->registry.emplace<TAG_GameLayer>(quad);

            auto mesh = std::make_shared<Mesh>(vertices, indices);
            mesh->SetData();
            m_Scene->registry.emplace<COMPMesh>(quad, mesh);

            HierarchySystem::PutInHierarchy(m_WorldHierarchy, ModelEntity);
            HierarchySystem::SetChildren(m_WorldHierarchy, ModelEntity, quad);
            static int x = 0;
            Transform_a.Move({x / 2 , x / 2, x / 2});
            x++;
        }
    }

    if (e.GetType() == MouseMoveEvent::GetStaticType())
    {
        if (!m_CanMoveMouse) return;
        if (m_Scene->m_CameraManager.GetActiveCamera() == entt::null) return;

        auto& mouse = dynamic_cast<MouseMoveEvent&>(e);
        auto& cam = m_Scene->registry.get<COMPCamera>(m_Scene->m_CameraManager.GetActiveCamera());

        static constexpr float sensitivity = 0.1f;
        static bool firstMouseInput = true;
        static double lastMouseX;
        static double lastMouseY;

        if (firstMouseInput)
        {
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
