#pragma once
#include <entt.hpp>

#include <Engine/Layer.hpp>
#include <Components/Drawable.hpp>
#include <Components/Camera.hpp>
#include <Components/Transform.hpp>
#include <Components/Material.hpp>
#include <Components/Mesh.hpp>
#include <Tags.hpp>
#include <Engine/Renderer.hpp>
#include <Engine/LowLevel/Shader.hpp>
#include <Engine/Events/WindowResizeEvent.hpp>

class GameLayer : public Layer
{
public:
    GameLayer() = default;
    ~GameLayer() = default;

    void OnDraw(Scene& scene, entt::registry& registry) override;
    void OnUpdate(entt::registry& registry, float dt) override;
    void OnAttach(Scene& scene, entt::registry& registry) override;
    void OnDetach(entt::registry& registry) override;
    void OnEvent(entt::registry& registry, Event& e) override;
};