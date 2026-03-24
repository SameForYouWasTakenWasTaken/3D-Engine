#pragma once
#include <entt.hpp>

#include <Engine/Layer.hpp>
#include <Components/Drawable.hpp>
#include <Components/Camera.hpp>
#include <Components/Transform.hpp>
#include <Components/Material.hpp>
#include <Components/Light.hpp>
#include <Components/Mesh.hpp>
#include <Tags.hpp>
#include <Engine/Renderer.hpp>
#include <Engine/LowLevel/Shader.hpp>
#include <Engine/Events/WindowResizeEvent.hpp>
#include <Engine/Events/TestEvent.hpp>
#include <Engine/Events/KeyInputEvent.hpp>
#include <Engine/Events/MouseMoveEvent.hpp>

#include <Engine/Systems/InputSystem.hpp>

#include "Engine/Systems/HierarchySystem.hpp"

class GameLayer : public Layer
{
    bool m_CanMoveMouse = false;
    Hierarchy m_WorldHierarchy;
public:
    GameLayer() = default;
    ~GameLayer() = default;

    void OnDraw() override;
    void OnUpdate(float dt) override;
    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Event& e) override;

    void SetMaterialOverrides(COMPModel& modelComponent, const MaterialOverride& materialOverride);
    void UpdateWorld(); // Update transform components
    void UpdateTransforms(entt::entity entity, const glm::mat4& inheritedWorld);
};
