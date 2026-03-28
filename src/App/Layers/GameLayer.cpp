#include "GameLayer.hpp"

#include "Engine/Systems/Services.hpp"
#include "Engine/Components/Model.hpp"

#include "Engine/Systems/AssetManager.hpp"

#include "Engine/Systems/HierarchySystem.hpp"
#include "vendor/Logger.hpp"

#include "App.hpp"

#include <tracy/Tracy.hpp>

#include "Engine/Engine.hpp"
#include "Engine/Systems/MeshManager.hpp"
#include "Engine/Systems/Texture2DManager.hpp"

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
    auto& sceneContext = m_Scene->GetContext();
    auto* cam = sceneContext.registry.try_get<COMPCamera>(sceneContext.m_CameraManager.GetActiveCamera());
    auto* camTransform = sceneContext.registry.try_get<COMPTransform>(sceneContext.m_CameraManager.GetActiveCamera());


    if (cam == nullptr || camTransform == nullptr)
    {
        logger.AppendLogTag("GAMELAYER", LogColors::GREEN);
        logger.AppendLogTag("CAMERA_MANAGER", LogColors::YELLOW);
        logger.LogWarning("No active camera set, skipping draw.");
        return;
    }
    auto& context = Engine::GetContext();

    auto generalView = sceneContext.registry.view<
        COMPMesh,
        COMPMaterial,
        COMPTransform
    >();

    auto modelView = sceneContext.registry.view<COMPModel, COMPTransform>();
    generalView.each([&](auto entity,
                         COMPMesh& mesh,
                         COMPMaterial& material,
                         COMPTransform& transform)
    {
        SubmitObject submit{};
        submit.meshID = mesh.mesh;
        submit.context = &sceneContext;
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
        auto* MatOverride = sceneContext.registry.try_get<MaterialOverride>(entity);
        if (MatOverride)
            SetMaterialOverrides(compModel, *MatOverride);

        int countMesh = 0;
        for (const auto& subMesh : subMeshes)
        {
            MaterialID materialID = subMesh.material;
            // Set mandatory data
            SubmitObject submit{};
            submit.meshID = subMesh.mesh;
            submit.context = &sceneContext;
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
            cloneMat.data.transparency = materialOverride.transparency.value();

        if (hasShininess)
            cloneMat.data.shininess = materialOverride.shininess.value();

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
    auto& sceneContext = m_Scene->GetContext();
    auto& registry = sceneContext.registry;

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

void GameLayer::OnResize(const WindowResizeEvent& resize)
{
    auto& sceneContext = m_Scene->GetContext();
    const float ratio = static_cast<float>(resize.Width) / static_cast<float>(resize.Height);
    auto view = sceneContext.registry.view<COMPCamera>();
    if (auto* cam = sceneContext.registry.try_get<COMPCamera>(view.front()))
    {
        cam->SetRatio(ratio);
    }

    std::cout << "Resize: " << resize.Width << ", " << resize.Height << std::endl;
}

void GameLayer::OnMouseMove(MouseMoveEvent& mouse)
{
    auto& sceneContext = m_Scene->GetContext();

    static constexpr float sensitivity = 0.1f;
    static bool firstMouseInput = true;
    static double lastMouseX;
    static double lastMouseY;

    if (!m_CanMoveMouse) return;
    if (sceneContext.m_CameraManager.GetActiveCamera() == entt::null) return;

    auto& cam = sceneContext.registry.get<COMPCamera>(sceneContext.m_CameraManager.GetActiveCamera());

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

void GameLayer::OnKey(KeyInputEvent& input)
{
    auto& asset_manager = Services::Get().GetService<AssetManager>();
    auto& shader_manager = Services::Get().GetService<ShaderManager>();
    auto& texture_manager = Services::Get().GetService<Texture2DManager>();
    auto& material_manager = Services::Get().GetService<MaterialManager>();
    auto& mesh_manager = Services::Get().GetService<MeshManager>();

    auto& sceneContext = m_Scene->GetContext();

    if (input.IsKey(GLFW_KEY_ESCAPE) && input.IsKeyPressed())
    {
        auto& engineContext = Engine::GetContext();

        int screenWidth = engineContext.WindowWidth;
        int screenHeight = engineContext.WindowHeight;
        static bool firstCapture = true;
        static bool clicked = false;
        static double LastMouseX = 0, LastMouseY = 0;

        if (!clicked)
        {
            clicked = true;
            glfwSetCursorPos(input.Window, LastMouseX, LastMouseY);
            glfwGetCursorPos(input.Window, &LastMouseX, &LastMouseY);

            glfwSetInputMode(input.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(input.Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            m_CanMoveMouse = true;
        }
        else
        {
            clicked = false;
            glfwSetCursorPos(input.Window, LastMouseX, LastMouseY);
            glfwGetCursorPos(input.Window, &LastMouseX, &LastMouseY);

            glfwSetInputMode(input.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetInputMode(input.Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            m_CanMoveMouse = false;
        }
    }


    if (input.IsKey(GLFW_KEY_F) && input.IsKeyPressed())
    {
        auto cam = sceneContext.m_CameraManager.GetActiveCamera();
        auto& camTransform = sceneContext.registry.get<COMPTransform>(cam);
        auto& camComponent = sceneContext.registry.get<COMPCamera>(cam);

        auto shadersDir = std::string(SHADERS_DIRECTORY);
        auto texturesDir = std::string(TEXTURES_DIRECTORY);
        auto objectsDir = std::string(OBJECTS_DIRECTORY);

        ModelID model_id = asset_manager.Load(objectsDir + "Spiderman/scene.gltf");
        // model
        auto ModelEntity = sceneContext.registry.create();
        auto& Model = sceneContext.registry.emplace<COMPModel>(ModelEntity, model_id);
        auto& Transform = sceneContext.registry.emplace<COMPTransform>(ModelEntity);
        auto& material = sceneContext.registry.emplace<MaterialOverride>(ModelEntity);
        material.transparency = 4.f;

        sceneContext.registry.emplace<TAG_GameLayer>(ModelEntity);

        Transform.SetPosition(camTransform.LocalPosition + camComponent.GetForward());
        Transform.Scale({0.5f, 0.5f, 0.5f});

        // second quad
        // shaders
        auto shader = shader_manager.Load(shadersDir + "first.vert", shadersDir + "first.frag");
        auto sunShader = shader_manager.Load(shadersDir + "first.vert", shadersDir + "sun.frag");

        auto basic_texture = texture_manager.Load(texturesDir + "images.png");

        // Materials
        auto mat = Material{shader, basic_texture.value()};

        MaterialID matID = material_manager.Load(mat);

        auto [vertices, indices] = GetDefault();
        auto quad = sceneContext.registry.create();
        auto& Transform_a = sceneContext.registry.emplace<COMPTransform>(quad);
        auto& Material_a = sceneContext.registry.emplace<COMPMaterial>(quad, matID);
        sceneContext.registry.emplace<TAG_GameLayer>(quad);

        auto mesh = Mesh(vertices, indices);
        mesh.SetData();

        auto meshID = mesh_manager.Load(std::move(mesh));

        sceneContext.registry.emplace<COMPMesh>(quad, meshID);

        HierarchySystem::PutInHierarchy(m_WorldHierarchy, ModelEntity);
        HierarchySystem::SetChildren(m_WorldHierarchy, ModelEntity, quad);
        static int x = 0;
        Transform_a.Move({x / 2, x / 2, x / 2});
        x++;
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
    auto& sceneContext = m_Scene->GetContext();
    UpdateWorld();
    // Camera movement
    auto active_cam = sceneContext.m_CameraManager.GetActiveCamera();
    auto* transform = sceneContext.registry.try_get<COMPTransform>(active_cam);
    auto* cam = sceneContext.registry.try_get<COMPCamera>(active_cam);

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
    auto& sceneContext = m_Scene->GetContext();
    auto [vertices, indices] = GetDefault();
    auto& services = Services::Get();

    auto& shader_manager = services.GetService<ShaderManager>();
    auto& material_manager = services.GetService<MaterialManager>();
    auto& texture_manager = services.GetService<Texture2DManager>();
    auto& asset_manager = services.GetService<AssetManager>();
    auto& light_manager = sceneContext.m_LightManager;
    auto& mesh_manager = services.GetService<MeshManager>();

    std::string shadersDir = std::string(SHADERS_DIRECTORY);
    std::string texturesDir = std::string(TEXTURES_DIRECTORY);
    std::string objectsDir = std::string(OBJECTS_DIRECTORY);
    // shaders

    auto shader = shader_manager.Load(shadersDir + "first.vert", shadersDir + "first.frag");
    auto sunShader = shader_manager.Load(shadersDir + "first.vert", shadersDir + "sun.frag");

    // textures
    auto basic_texture = texture_manager.Load(texturesDir + "images.png");
    auto color_grid_texture = texture_manager.Load(texturesDir + "default.png");

    // Materials
    auto mat = Material{shader, basic_texture.value()};
    auto sunMat = Material{sunShader, color_grid_texture.value()};

    MaterialID matID = material_manager.Load(mat);
    MaterialID MaterialID_colorgrid = material_manager.Load(sunMat);
    // first quad
    auto quad_1 = sceneContext.registry.create();
    auto& Transform = sceneContext.registry.emplace<COMPTransform>(quad_1);
    auto& Material = sceneContext.registry.emplace<COMPMaterial>(quad_1, matID);

    sceneContext.registry.emplace<TAG_GameLayer>(quad_1);
    std::vector<COMPTexture> textures;
    textures.push_back(COMPTexture{basic_texture.value()});

    auto mesh = Mesh(vertices, indices);
    mesh.SetData();

    auto meshID = mesh_manager.Load(std::move(mesh));
    auto& ComponentMesh = sceneContext.registry.emplace<COMPMesh>(quad_1, meshID);


    Transform.SetPosition({0.f, 0.f, 0.f});


    // second quad
    auto quad_2 = sceneContext.registry.create();
    auto& Transform_a = sceneContext.registry.emplace<COMPTransform>(quad_2);
    auto& Material_a = sceneContext.registry.emplace<COMPMaterial>(quad_2, matID);
    sceneContext.registry.emplace<TAG_GameLayer>(quad_2);
    sceneContext.registry.emplace<COMPMesh>(quad_2, meshID);

    Transform_a.SetPosition({3.f, 1.f, -2.f});


    // MODELS
    auto defModelID = asset_manager.Load(objectsDir + "Spiderman/scene.gltf");

    auto ironman = sceneContext.registry.create();

    auto& Transf = sceneContext.registry.emplace<COMPTransform>(ironman);
    auto& model = sceneContext.registry.emplace<COMPModel>(ironman, defModelID);
    Transf.Scale({0.5, 0.5f, 0.5f});

    // THE SUNS
    // i dont need many if checks, fairly confident this'll work
    // Light object
    auto LightEntity = sceneContext.registry.create();
    auto& lightTransform = sceneContext.registry.emplace<COMPTransform>(LightEntity);
    auto& lightMaterial = sceneContext.registry.emplace<COMPMaterial>(LightEntity, matID);

    sceneContext.registry.emplace<TAG_GameLayer>(LightEntity);

    auto lightMesh = Mesh(vertices, indices);
    lightMesh.SetData();
    auto lightMeshID = mesh_manager.Load(std::move(lightMesh));
    sceneContext.registry.emplace<COMPMesh>(LightEntity, lightMeshID);

    auto LightObject = sceneContext.registry.create();
    auto lightID = light_manager.Load<PointLight>();
    auto LightType = light_manager.GetLight<PointLight>(lightID.value());

    LightType->data.position = {0.f, 0.f, 0.f};

    HierarchySystem::PutInHierarchy(m_WorldHierarchy, quad_1);
    HierarchySystem::PutInHierarchy(m_WorldHierarchy, quad_2);
    HierarchySystem::PutInHierarchy(m_WorldHierarchy, ironman);
    HierarchySystem::PutInHierarchy(m_WorldHierarchy, LightEntity);
    HierarchySystem::SetChildren(m_WorldHierarchy, LightEntity, LightObject);

    EngineContext& engineContext = Engine::GetContext();

    auto cam_entity = sceneContext.registry.create();
    auto& cam_transform = sceneContext.registry.emplace<COMPTransform>(cam_entity);
    cam_transform.SetPosition({0.f, 0.f, -2.f});

    float ratio = static_cast<float>(engineContext.WindowWidth) / static_cast<float>(engineContext.WindowHeight);

    sceneContext.registry.emplace<COMPCamera>(cam_entity, 90.f, ratio, 0.01f, 1000.f);
    sceneContext.m_CameraManager.SetActiveCamera(cam_entity);

    // Connect events
    engineContext.EventDispatcher.sink<WindowResizeEvent>().connect<&GameLayer::OnResize>(this);
    engineContext.EventDispatcher.sink<KeyInputEvent>().connect<&GameLayer::OnKey>(this);
    engineContext.EventDispatcher.sink<MouseMoveEvent>().connect<&GameLayer::OnMouseMove>(this);
}

void GameLayer::OnDetach()
{
    auto& sceneContext = m_Scene->GetContext();
    auto& engineContext = Engine::GetContext();
    auto view = sceneContext.registry.view<TAG_GameLayer>();
    view.each([&](auto entity)
    {
        sceneContext.registry.destroy(entity);
    });

    engineContext.EventDispatcher.sink<WindowResizeEvent>().disconnect<&GameLayer::OnResize>(this);
    engineContext.EventDispatcher.sink<KeyInputEvent>().disconnect<&GameLayer::OnKey>(this);
    engineContext.EventDispatcher.sink<MouseMoveEvent>().disconnect<&GameLayer::OnMouseMove>(this);
}
