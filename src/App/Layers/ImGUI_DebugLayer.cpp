#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ImGUI_DebugLayer.hpp"

#include "Engine/Systems/AssetManager.hpp"

#include <cmath>

#include "Engine/Engine.hpp"
#include "Engine/Events/WindowResizeEvent.hpp"
#include "tracy/Tracy.hpp"

#define TYPE_TO_STRING(x) #x

/**
 * @brief Produces a compact, human-readable string for large integer counts using suffixes.
 *
 * Converts the given integer into a decimal with one digit after the decimal point and appends a suffix from "", "K", "M", "B", "T" to denote thousands, millions, billions, and trillions (for example, 1234 -> "1.2K").
 *
 * @param value The numeric value to format.
 * @return std::string Shortened representation of the value with one decimal place and an appropriate suffix.
 */
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

/**
 * @brief Releases ImGui resources and shuts down its GLFW/OpenGL backends.
 *
 * Cleans up the ImGui OpenGL3 and GLFW platform backends and destroys the ImGui context to free associated resources.
 */
ImGUI_DebugLayer::~ImGUI_DebugLayer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/**
 * @brief Renders the debug UI and submits ImGui draw data to OpenGL.
 *
 * Starts a new ImGui frame, builds the "DEBUG" window by invoking CameraGUI() and WorldGUI(),
 * then finalizes ImGui and submits the resulting draw data to the OpenGL3 backend.
 */
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

/**
 * @brief Per-frame update hook for the debug layer.
 *
 * This implementation performs no per-frame logic; the method is a no-op.
 *
 * @param deltaTime Time elapsed since the previous frame, in seconds.
 */
void ImGUI_DebugLayer::OnUpdate(float deltaTime)
{
}

/**
 * @brief Adjusts ImGui font and style scale when the window is resized.
 *
 * When the provided event is a WindowResizeEvent, computes a scale factor as
 * resize.Width / 1280.0 and applies it to ImGui's global font scale and style
 * (resetting the style to defaults before scaling).
 *
 * @param event The incoming event; only WindowResizeEvent instances affect ImGui scaling.
 */
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

/**
 * @brief Initialize ImGui context and backends for the current scene's GLFW OpenGL window.
 *
 * Creates an ImGui context, enables keyboard and gamepad navigation, retrieves the scene's active
 * GLFW window, and initializes the ImGui GLFW and OpenGL3 backends. If no active window is
 * available, the function returns without initializing backends.
 */
void ImGUI_DebugLayer::OnAttach()
{
    auto& sceneContext = m_Scene->GetContext();
    GLFWwindow* window = Engine::GetContext().ActiveWindow;
    if (!window)
        ImGuiEnable = false;

    if (!ImGuiEnable) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

/**
 * @brief Called when the debug layer is detached from the application.
 *
 * Currently performs no teardown or state changes.
 */
void ImGUI_DebugLayer::OnDetach()
{
}

/**
 * @brief Renders the Camera section of the ImGui debug window and exposes interactive camera controls.
 *
 * Displays a "Camera settings" header and an "Open" toggle; if no active camera or transform is available it shows "No valid camera exists!" and returns.
 * When opened, presents controls for FOV, near and far planes (updating the camera and setting `dirty_camera = true` on change), shows forward/right/up vectors, world position, yaw and pitch, and provides a "Reset Camera" button that sets yaw and pitch to 0.0 and the camera transform position to {0.0f, 0.0f, 3.0f} (also marking the camera dirty).
 */
void ImGUI_DebugLayer::CameraGUI()
{
    auto& sceneContext = m_Scene->GetContext();
    if (!ImGuiEnable) return;
    static bool show = false;
    auto activeCam = sceneContext.m_CameraManager.GetActiveCamera();
    auto* Camera = sceneContext.registry.try_get<COMPCamera>(activeCam);
    auto* CameraTransform = sceneContext.registry.try_get<COMPTransform>(activeCam);

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
        glm::vec3 position = CameraTransform->LocalPosition;

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
            CameraTransform->LocalPosition = {0.0f, 0.0f, 3.0f};
            Camera->dirty_camera = true;
        }
    }
}

/**
 * @brief Renders the "World settings" ImGui panel and allows inspecting and modifying world and render-state parameters.
 *
 * Displays a toggleable UI section that:
 * - Shows counts for meshes, models, and rendering statistics.
 * - Enumerates models to include their sub-meshes when computing mesh count.
 * - Exposes controls to modify render state stored in the scene context's StateCache (VSync, wireframe, depth test/write, depth function,
 *   stencil test, color blending, face culling, cull mode, and winding).
 *
 * The function reads engine services (AssetManager and Renderer) and the active scene context; it updates Context->StateCache in response
 * to user interactions. If no valid context exists, a message is shown and no state is modified.
 */
void ImGUI_DebugLayer::WorldGUI()
{
    if (!ImGuiEnable) return;
    static bool show = false;

    ImGui::Text("World settings");
    // Activate / deactivate
    if (ImGui::Button("Open##2"))
        show = !show;

    if (show)
    {
        auto& sceneContext = m_Scene->GetContext();
        auto& engineContext = Engine::GetContext();
        auto& assetManager = Services::Get().GetService<AssetManager>();
        auto& renderer = Services::Get().GetService<Renderer>();
        auto MeshView = sceneContext.registry.view<COMPMesh>();
        auto ModelView = sceneContext.registry.view<COMPModel>();

        size_t MeshCount = MeshView.size();
        size_t ModelCount = ModelView.size();

        ModelView.each([&](auto entity, COMPModel& model)
        {
            auto Model = assetManager.Get(model.id);
            auto SubMeshes = Model->GetSubMeshes();
            for (const auto& mesh : SubMeshes)
            {
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
            bool v = OptionToBool(engineContext.StateCache.VSync);
            if (ImGui::Checkbox("VSync", &v))
                engineContext.StateCache.VSync = BoolToOption(v);
        }

        // --- Wireframe ---
        {
            bool v = OptionToBool(engineContext.StateCache.Wireframe);
            static bool toggled = false;
            if (ImGui::Checkbox("Wireframe", &v))
            {
                static bool CullingOption = OptionToBool(engineContext.StateCache.Culling);
                static bool DepthOption = OptionToBool(engineContext.StateCache.DepthTest);
                engineContext.StateCache.Wireframe = BoolToOption(v);
                engineContext.StateCache.Culling = BoolToOption(false);
                engineContext.StateCache.DepthTest = BoolToOption(false);

                if (toggled)
                {
                    engineContext.StateCache.Culling = BoolToOption(CullingOption);
                    engineContext.StateCache.DepthTest = BoolToOption(DepthOption);
                }

                toggled = !toggled;
            }
        }

        // --- Depth Test ---
        {
            bool v = OptionToBool(engineContext.StateCache.DepthTest);
            if (ImGui::Checkbox("Depth Test", &v))
                engineContext.StateCache.DepthTest = BoolToOption(v);
        }

        // --- Depth Write ---
        {
            bool v = (engineContext.StateCache.DepthWrite == DEPTH_WRITE::ENABLED);
            if (ImGui::Checkbox("Depth Write", &v))
                engineContext.StateCache.DepthWrite = v ? DEPTH_WRITE::ENABLED : DEPTH_WRITE::DISABLED;
        }

        // --- Depth Function ---
        {
            int current = static_cast<int>(engineContext.StateCache.DepthFunc);

            const char* depthFuncs[] = {
                "NEVER", "LESS", "EQUAL", "LEQUAL",
                "GREATER", "NOTEQUAL", "GEQUAL", "ALWAYS"
            };

            if (ImGui::Combo("Depth Func", &current, depthFuncs, IM_ARRAYSIZE(depthFuncs)))
            {
                engineContext.StateCache.DepthFunc = static_cast<DEPTH_FUNC>(current);
            }
        }

        // --- Stencil Test ---
        {
            bool v = OptionToBool(engineContext.StateCache.StencilTest);
            if (ImGui::Checkbox("Stencil Test", &v))
                engineContext.StateCache.StencilTest = BoolToOption(v);
        }

        // --- Blending ---
        {
            bool v = OptionToBool(engineContext.StateCache.ColorBlend);
            if (ImGui::Checkbox("Color Blending", &v))
                engineContext.StateCache.ColorBlend = BoolToOption(v);
        }

        // --- Face Culling ---
        {
            bool v = OptionToBool(engineContext.StateCache.Culling);
            if (ImGui::Checkbox("Face Culling", &v))
            {
                engineContext.StateCache.Culling = BoolToOption(v);
            }
        }

        // --- Cull Mode ---
        if (engineContext.StateCache.Culling == OPTION::YES)
        {
            int current = static_cast<int>(engineContext.StateCache.CullingMode);

            const char* modes[] = {"BACK", "FRONT", "FRONT_AND_BACK"};

            if (ImGui::Combo("Cull Mode", &current, modes, IM_ARRAYSIZE(modes)))
            {
                engineContext.StateCache.CullingMode = static_cast<CULLING_MODE>(current);
            }

            int winding = static_cast<int>(engineContext.StateCache.WindingDir);

            const char* windings[] = {"CLOCKWISE", "COUNTER_CLOCKWISE"};

            if (ImGui::Combo("Winding", &winding, windings, IM_ARRAYSIZE(windings)))
            {
                engineContext.StateCache.WindingDir = static_cast<WINDING_DIR>(winding);
            }
        }

        ImGui::Separator();
        RenderStats& stats = renderer.g_RenderStats;

        ImGui::Text("Mesh count: %s", FormatShort(MeshCount).c_str());
        ImGui::Text("Model count: %s", FormatShort(ModelCount).c_str());
        ImGui::Text("Total vertices: %s", FormatShort(stats.verticesSubmitted).c_str());
        ImGui::Text("Total triangles: %s", FormatShort(stats.trianglesSubmitted).c_str());
        ImGui::Text("Total indices: %s", FormatShort(stats.indicesSubmitted).c_str());
        ImGui::Text("Total draw calls: %u", stats.drawCalls);
    }
}
