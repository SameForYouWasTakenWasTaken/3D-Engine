#pragma once

#include <memory>
#include <unordered_map>
#include <functional>

#include <App/Services.hpp>
#include <entt.hpp>
#include <Engine/Scene.hpp>
#include <Contexts/EngineContext.hpp>
#include <Logger.hpp>
#include <Engine/Renderer.hpp>
#include <Engine/Events.hpp>

using EventCallbackFn = std::function<void(Event&)>;
class Scene; // forward declaration
class EngineContext; // forward declaration

class SceneManager : public IService
{
    uint32_t m_NextSceneID = 0;
    EventCallbackFn m_EventCallback;
    std::unordered_map<uint32_t, std::shared_ptr<Scene>> m_Scenes;
public:
    EngineContext& m_EngineContext;
    std::shared_ptr<Scene> m_ActiveScene = nullptr;

    Logger logger = Logger("SCENE_MANAGER");
    
    SceneManager(EngineContext& context) : m_EngineContext(context) {};
    ~SceneManager() = default;

    void Draw();
    void Update(float dt);
    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(uint32_t sceneID);
    void ChangeScene(uint32_t sceneID);
    void NextScene();
    void OnEvent(Event& e);
    void SetEventCallback(const EventCallbackFn& callback);

    std::shared_ptr<Scene> GetSceneFromID(uint32_t id);
};