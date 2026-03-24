#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Engine/Layer.hpp"

class ImGUI_DebugLayer : public Layer
{

public:
    /**
 * @brief Constructs an ImGUI_DebugLayer used to render and manage the debug ImGui interface.
 *
 * Initializes an instance of the ImGUI debug layer; platform and renderer bindings are established when OnAttach() is called.
 */
ImGUI_DebugLayer() = default;
    ~ImGUI_DebugLayer() override;

    void OnDraw() override;
    void OnUpdate(float deltaTime) override;
    void OnEvent(Event& event) override;
    void OnAttach() override;
    void OnDetach() override;

    void CameraGUI(); // Setup camera GUI
    void WorldGUI(); // Setup world stuff
};
