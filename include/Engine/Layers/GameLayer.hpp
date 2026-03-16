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

class GameLayer : public Layer
{
    bool m_CanMoveMouse = false;
public:
    GameLayer() = default;
    ~GameLayer() = default;

    void OnDraw() override;
    void OnUpdate(float dt) override;
    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Event& e) override;
};