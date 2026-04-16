#pragma once
#include <vendor/entt.hpp>

#include "../../Core/Engine/Renderer/API/Screen/Layer.hpp"
#include "Engine/Components/Camera.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/Components/Material.hpp"
#include "Engine/Components/Light.hpp"
#include "Engine/Components/Mesh.hpp"
#include "Engine/Components/Tags.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/API/Shader.hpp"
#include <Engine/Events/WindowResizeEvent.hpp>
#include <Engine/Events/TestEvent.hpp>
#include <Engine/Events/KeyInputEvent.hpp>
#include <Engine/Events/MouseMoveEvent.hpp>

#include "Engine/Components/Model.hpp"
#include "Engine/Systems/InputSystem.hpp"

#include "Engine/Systems/HierarchySystem.hpp"

class GameLayer : public Layer
{
    bool m_CanMoveMouse = false;
    double m_MouseDeltaX{};
    double m_MouseDeltaY{};
    bool m_FirstMouseInput = true;
    double m_LastMouseX = 0.0;
    double m_LastMouseY = 0.0;

    Hierarchy m_WorldHierarchy;
public:
    GameLayer() = default;
    ~GameLayer() = default;

    void OnDraw() override;
    void OnUpdate(float dt) override;
    void OnAttach() override;
    void OnDetach() override;

    void SetMaterialOverrides(COMPModel& modelComponent, const MaterialOverride& materialOverride);
    void UpdateWorld(); // Update transform components
    void UpdateTransforms(entt::entity entity, const glm::mat4& inheritedWorld);

    void OnResize(const WindowResizeEvent& resize);
    void OnMouseMove(MouseMoveEvent& mouse);
    void OnKey(KeyInputEvent& input);
};
