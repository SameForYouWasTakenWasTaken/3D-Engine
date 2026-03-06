#pragma once

#include <queue>

#include <Engine/Layer.hpp>
#include <Logger.hpp>

#include <entt.hpp>
#include <Engine/Systems/CameraManager.hpp>
#include <Engine/Systems/SceneManager.hpp>
#include <Engine/Renderer.hpp>

#include <Components/Drawable.hpp>
#include <Components/Transform.hpp>
#include <Components/Material.hpp>
#include <Components/Mesh.hpp>
#include <Engine/Events.hpp>
#include <Engine/Events/WindowResizeEvent.hpp>

class Layer; // forward declaration
class Renderer; // forward declaration
class SceneManager; // forward declaration
class EngineContext; // forward declaration
class CameraManager;

class Scene final
{
    SceneManager* m_SceneManager;
    std::vector<std::shared_ptr<Layer>> m_Layers;
public:
    Logger logger = Logger("SCENE");
    CameraManager m_CameraManager;
    
    entt::registry registry;

    Scene() = default;
    ~Scene() = default;


    virtual void AddLayer(std::shared_ptr<Layer> layer);
    virtual void RemoveLayer(std::shared_ptr<Layer> layer);
    virtual void Draw();
    virtual void Update(float dt);
    virtual void OnEvent(Event& e);

    void SetSceneManager(SceneManager* sceneManager);
    EngineContext& GetContext();
};