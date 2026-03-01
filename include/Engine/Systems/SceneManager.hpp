#pragma once

#include <memory>
#include <vector>

#include <entt.hpp>
#include <Engine/Scene.hpp>
#include <Contexts/EngineContext.hpp>
#include <Logger.hpp>

class Scene; // forward declaration
class SceneManager final
{
    std::vector<std::shared_ptr<Scene>> m_Scenes;
public:
    EngineContext& m_EngineContext;
    std::shared_ptr<Scene> m_ActiveScene = nullptr;

    Logger logger = Logger("SCENE_MANAGER");
    
    SceneManager(EngineContext& context) : m_EngineContext(context) {};
    ~SceneManager() = default;

    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(std::shared_ptr<Scene> scene);
    void ChangeScene(std::shared_ptr<Scene> scene);
    void NextScene();
};