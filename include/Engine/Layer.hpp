#pragma once

#include <vector>
#include <entt.hpp>
#include <Engine/Scene.hpp>

class Scene; // forward declaration

class Layer {
public:
    Layer() = default;
    ~Layer() = default;

    virtual void OnDraw(Scene& scene, entt::registry& registry) {}
    virtual void OnUpdate(entt::registry& registry, float dt) {}
    virtual void OnEvent(entt::registry& registry) {} //TODO: add event to arguments later
    virtual void OnAttach(Scene& scene, entt::registry& registry) {}
    virtual void OnDetach(entt::registry& registry) {}
};