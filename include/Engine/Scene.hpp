#pragma once

#include <Engine/Layer.hpp>
#include <Logger.hpp>

#include <entt.hpp>
#include <Engine/Systems/CameraManager.hpp>
#include <Engine/Renderer.hpp>

#include <Components/Drawable.hpp>
#include <Components/Transform.hpp>
#include <Components/Material.hpp>
#include <Components/Mesh.hpp>

class Layer; // forward declaration
class Renderer; // forward declaration

class Scene final
{
    std::vector<std::shared_ptr<Layer>> m_Layers;
    entt::registry m_Registry;
public:
    EngineContext& m_EngineContext;
    CameraManager m_CameraManager;
    Logger logger = Logger("SCENE");

    Scene(EngineContext& context) : m_EngineContext(context), m_CameraManager(context) {};
    ~Scene() = default;


    virtual void AddLayer(std::shared_ptr<Layer> layer);
    virtual void RemoveLayer(std::shared_ptr<Layer> layer);
    virtual void Draw(Renderer& renderer);
    virtual void Update(float dt);
};