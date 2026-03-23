#include <Engine/Layers/ImGUI_DebugLayer.hpp>

#include "Engine/Systems/AssetManager.hpp"

#include <cmath>

std::string FormatShort(size_t value)
{
    const char* suffixes[] = {"", "K", "M", "B", "T"};
    double count = static_cast<double>(value);

    int i = 0;
    while (count >= 1000.0 && i < 4)
    {
        count /= 1000.0;
        i++;
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.1f%s", count, suffixes[i]);
    return buffer;
}

ImGUI_DebugLayer::~ImGUI_DebugLayer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGUI_DebugLayer::OnDraw()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("DEBUG");
    CameraGUI();
    WorldGUI();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGUI_DebugLayer::OnUpdate(float deltaTime)
{
}

void ImGUI_DebugLayer::OnEvent(Event& event)
{
    if (event.GetType() == WindowResizeEvent::GetStaticType())
    {
        auto& resize = static_cast<WindowResizeEvent&>(event);
        float scale = resize.Width / 1280.f;

        ImGuiIO& io = ImGui::GetIO();
        io.FontGlobalScale = scale;

        ImGuiStyle& style = ImGui::GetStyle();
        style = ImGuiStyle(); //reset
        style.ScaleAllSizes(scale);
    }
}

void ImGUI_DebugLayer::OnAttach()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    GLFWwindow* window = m_Scene->GetContext().value()->ActiveWindow;

    if (!window)
        return;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void ImGUI_DebugLayer::OnDetach()
{
}

void ImGUI_DebugLayer::CameraGUI()
{
    static bool show = false;
    auto activeCam = m_Scene->m_CameraManager.GetActiveCamera();
    auto* Camera = m_Scene->registry.try_get<COMPCamera>(activeCam);
    auto* CameraTransform = m_Scene->registry.try_get<COMPTransform>(activeCam);

    if (!Camera || !CameraTransform)
    {
        ImGui::Text("No valid camera exists!");
        return;
    }

    ImGui::Text("Camera settings");
    // Activate / deactivate
    if (ImGui::Button("Open##1"))
        show = !show;

    // Show the main GUI?
    if (show)
    {
        float fov = Camera->GetCameraContext().fov;
        float nearPlane = Camera->GetCameraContext().near;
        float farPlane = Camera->GetCameraContext().far;
        glm::vec3 forward = Camera->GetForward();
        glm::vec3 right = Camera->GetRight();
        glm::vec3 up = Camera->GetUp();
        glm::vec3 position = CameraTransform->position;

        if (ImGui::SliderFloat("FOV", &fov, 30.0f, 120.0f))
        {
            Camera->SetFOV(fov);
            Camera->dirty_camera = true;
        }


        if (ImGui::DragFloat("Near", &nearPlane, 0.01f, 0.01f, 10.0f))
        {
            Camera->SetNear(nearPlane);
            Camera->dirty_camera = true;
        }

        if (ImGui::DragFloat("Far", &farPlane, 1.0f, 10.0f, 10000.0f))
        {
            Camera->SetFar(farPlane);
            Camera->dirty_camera = true;
        }

        ImGui::Text("Forward: %.2f %.2f %.2f", forward.x, forward.y, forward.z);
        ImGui::Text("Right:   %.2f %.2f %.2f", right.x, right.y, right.z);
        ImGui::Text("Up:      %.2f %.2f %.2f", up.x, up.y, up.z);
        ImGui::Text("Position: %.3f %.3f %.3f", position.x, position.y, position.z);
        ImGui::Text("yaw: %.2f", Camera->yaw);
        ImGui::Text("pitch: %.2f", Camera->pitch);

        if (ImGui::Button("Reset Camera"))
        {
            Camera->yaw = 0.0f;
            Camera->pitch = 0.0f;
            CameraTransform->position = {0.0f, 0.0f, 3.0f};
            Camera->dirty_camera = true;
        }
    }
}

void ImGUI_DebugLayer::WorldGUI()
{
    static bool show = false;

    ImGui::Text("World settings");
    // Activate / deactivate
    if (ImGui::Button("Open##2"))
        show = !show;

    if (show)
    {
        auto& assetManager = Services::Get().GetService<AssetManager>();
        auto MeshView = m_Scene->registry.view<COMPMesh>();
        auto ModelView = m_Scene->registry.view<COMPModel>();

        size_t MeshCount = MeshView.size();
        size_t ModelCount = ModelView.size();
        unsigned int TotalVertices = 0;

        auto* Context = m_Scene->GetContext().value();

        if (!Context)
        {
            ImGui::Text("No valid context exists!");
            return;
        }

        MeshView.each([&](auto entity, COMPMesh& mesh)
        {
            TotalVertices += mesh.mesh->vertices.size();
        });

        ModelView.each([&](auto entity, COMPModel& model)
        {
            auto Model = assetManager.Get(model.id);
            auto SubMeshes = Model->GetSubMeshes();

            for (const auto& mesh : SubMeshes)
            {
                TotalVertices += mesh.mesh->vertices.size();
                MeshCount++;
            }
        });

        // --- Render State Controls ---
        ImGui::Separator();
        ImGui::Text("Render States");

        // Helper lambda to convert OPTION <-> bool
        auto OptionToBool = [](OPTION o) { return o == OPTION::YES; };
        auto BoolToOption = [](bool b) { return b ? OPTION::YES : OPTION::NO; };

        // --- VSync ---
        {
            bool v = OptionToBool(Context->StateCache.VSync);
            if (ImGui::Checkbox("VSync", &v))
                Context->StateCache.VSync = BoolToOption(v);
        }

        // --- Wireframe ---
        {
            bool v = OptionToBool(Context->StateCache.Wireframe);
            static bool clicked = false;
            static bool CullingOption = OptionToBool(Context->StateCache.Culling);
            static bool DepthOption = OptionToBool(Context->StateCache.DepthTest);
            if (ImGui::Checkbox("Wireframe", &v))
            {
                Context->StateCache.Wireframe = BoolToOption(v);
                Context->StateCache.Culling = BoolToOption(false);
                Context->StateCache.DepthTest = BoolToOption(false);
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Disables culling and depth test");
            }

        }

        // --- Depth Test ---
        {
            bool v = OptionToBool(Context->StateCache.DepthTest);
            if (ImGui::Checkbox("Depth Test", &v))
                Context->StateCache.DepthTest = BoolToOption(v);
        }

        // --- Depth Write ---
        {
            bool v = (Context->StateCache.DepthWrite == DEPTH_WRITE::ENABLED);
            if (ImGui::Checkbox("Depth Write", &v))
                Context->StateCache.DepthWrite = v ? DEPTH_WRITE::ENABLED : DEPTH_WRITE::DISABLED;
        }

        // --- Depth Function ---
        {
            int current = static_cast<int>(Context->StateCache.DepthFunc);

            const char* depthFuncs[] = {
                "NEVER", "LESS", "EQUAL", "LEQUAL",
                "GREATER", "NOTEQUAL", "GEQUAL", "ALWAYS"
            };

            if (ImGui::Combo("Depth Func", &current, depthFuncs, IM_ARRAYSIZE(depthFuncs)))
            {
                Context->StateCache.DepthFunc = static_cast<DEPTH_FUNC>(current);
            }
        }

        // --- Stencil Test ---
        {
            bool v = OptionToBool(Context->StateCache.StencilTest);
            if (ImGui::Checkbox("Stencil Test", &v))
                Context->StateCache.StencilTest = BoolToOption(v);
        }

        // --- Blending ---
        {
            bool v = OptionToBool(Context->StateCache.ColorBlend);
            if (ImGui::Checkbox("Color Blending", &v))
                Context->StateCache.ColorBlend = BoolToOption(v);
        }

        // --- Face Culling ---
        {
            bool v = OptionToBool(Context->StateCache.Culling);
            if (ImGui::Checkbox("Face Culling", &v))
            {
                Context->StateCache.Culling = BoolToOption(v);
                Context->StateCache.Wireframe = BoolToOption(false);
            }
        }

        // --- Cull Mode ---
        if (Context->StateCache.Culling == OPTION::YES)
        {
            int current = static_cast<int>(Context->StateCache.CullingMode);

            const char* modes[] = {"BACK", "FRONT", "FRONT_AND_BACK"};

            if (ImGui::Combo("Cull Mode", &current, modes, IM_ARRAYSIZE(modes)))
            {
                Context->StateCache.CullingMode = static_cast<CULLING_MODE>(current);
            }

            int winding = static_cast<int>(Context->StateCache.WindingDir);

            const char* windings[] = {"CLOCKWISE", "COUNTER_CLOCKWISE"};

            if (ImGui::Combo("Winding", &winding, windings, IM_ARRAYSIZE(windings)))
            {
                Context->StateCache.WindingDir = static_cast<WINDING_DIR>(winding);
            }
        }

        ImGui::Separator();

        ImGui::Text("Mesh count: %s", FormatShort(MeshCount).c_str());
        ImGui::Text("Model count: %s", FormatShort(ModelCount).c_str());
        ImGui::Text("Total vertices: %s", FormatShort(TotalVertices).c_str());
        ImGui::Text("Total triangles: %u", TotalVertices / 3);
    }
}
