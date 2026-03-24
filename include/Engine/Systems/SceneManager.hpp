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
    /**
 * @brief Default-constructs a SceneManager.
 *
 * Constructs a SceneManager with default-initialized internal state and no active scene.
 */
SceneManager() = default;
    /**
 * @brief Disables copy construction of SceneManager.
 *
 * Copying is prohibited because SceneManager owns or references unique engine resources
 * (engine context, scene collection, active scene and logger) and must maintain a single
 * distinct instance.
 */
SceneManager(const SceneManager&) = delete;
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