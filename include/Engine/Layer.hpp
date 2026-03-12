#pragma once

#include <vector>
#include <entt.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Events.hpp>
#include <Logger.hpp>

class Scene; // forward declaration

class Layer {
protected:
    Scene* m_Scene = nullptr;
public:
    Logger logger = Logger("LAYER_BASE");
    Layer() = default;
    virtual ~Layer() = default;

    void SetScene(Scene* scene);
    void EmitEvent(Event& e);

    virtual void OnDraw() {}
    virtual void OnUpdate(float dt) {}
    virtual void OnEvent(Event& e) {} //TODO: add event to arguments later
    virtual void OnAttach() {}
    virtual void OnDetach() {}
};