#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <vendor/entt.hpp>

#include "Engine/Systems/Services.hpp"
#include "Contexts/EngineContext.hpp"
#include "vendor/Logger.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Events/Events.hpp"
#include "Engine/Renderer/API/Scene.hpp"
#include "Layers/Layer.hpp"

using EventCallbackFn = std::function<void(Event&)>;
class Scene; // forward declaration
class EngineContext; // forward declaration

class SceneManager : public IService
{
    uint32_t m_NextSceneID = 0;
    EventCallbackFn m_EventCallback;
    std::unordered_map<std::shared_ptr<Scene>, std::vector<std::shared_ptr<Layer>>> m_Scenes;

public:
    /**
 * @brief Disables copy construction of SceneManager.
 *
 * Copying is prohibited because SceneManager owns or references unique engine resources
 * (engine context, scene collection, active scene and logger) and must maintain a single
 * distinct instance.
 */
    SceneManager(const SceneManager&) = delete;
    std::shared_ptr<Scene> m_ActiveScene = nullptr;

    Logger logger = Logger("SCENE_MANAGER");

    SceneManager() = default;

    void Draw();
    void Update(float dt);
    uint32_t AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(std::shared_ptr<Scene> scene);
    void ChangeScene(std::shared_ptr<Scene> scene);
    void AddLayer(std::shared_ptr<Scene> scene, std::shared_ptr<Layer> layer);
    void RemoveLayer(std::shared_ptr<Scene> scene, std::shared_ptr<Layer> layer);
    void NextScene();
    void OnEvent(Event& e);
    void SetEventCallback(const EventCallbackFn& callback);

    std::shared_ptr<Scene> DoesSceneExist(std::shared_ptr<Scene> scene) const;
};
