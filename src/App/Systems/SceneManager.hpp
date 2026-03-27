#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <vendor/entt.hpp>

#include "Engine/Systems/Services.hpp"
#include "Engine/Renderer/API/Screen/Scene.hpp"
#include "Contexts/EngineContext.hpp"
#include "vendor/Logger.hpp"
#include "Engine/Renderer/Renderer.hpp"

using EventCallbackFn = std::function<void(Event&)>;

class SceneManager : public IService
{
    uint32_t m_NextSceneID = 0;
    EventCallbackFn m_EventCallback;
    std::unordered_map<uint32_t, std::shared_ptr<Scene>> m_Scenes;

public:
    SceneManager(const SceneManager&) = delete;
    SceneManager() = default;

    std::shared_ptr<Scene> m_ActiveScene = nullptr;

    Logger logger = Logger("SCENE_MANAGER");

    void Draw();
    void Update(float dt);
    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(uint32_t sceneID);
    void ChangeScene(uint32_t sceneID);
    void NextScene();
    void SetEventCallback(const EventCallbackFn& callback);

    std::shared_ptr<Scene> GetSceneFromID(uint32_t id);
};