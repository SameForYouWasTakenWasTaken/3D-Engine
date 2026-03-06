#pragma once

#include <memory>
#include <vector>
#include <functional>

#include <entt.hpp>
#include <Engine/Scene.hpp>
#include <Contexts/EngineContext.hpp>
#include <Logger.hpp>
#include <Engine/Renderer.hpp>
#include <Engine/Events.hpp>

using EventCallbackFn = std::function<void(Event&)>;
class Scene; // forward declaration
class EngineContext; // forward declaration
class SceneManager final
{
    EventCallbackFn m_EventCallback;
    std::vector<std::shared_ptr<Scene>> m_Scenes;
public:
    EngineContext& m_EngineContext;
    std::shared_ptr<Scene> m_ActiveScene = nullptr;

    Logger logger = Logger("SCENE_MANAGER");
    
    SceneManager(EngineContext& context) : m_EngineContext(context) {};
    ~SceneManager() = default;

    void Draw();
    void Update(float dt);
    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(std::shared_ptr<Scene> scene);
    void ChangeScene(std::shared_ptr<Scene> scene);
    void NextScene();
    void OnUpdate(Event& e);
    void OnEvent(Event& e);
    void SetEventCallback(const EventCallbackFn& callback);
};