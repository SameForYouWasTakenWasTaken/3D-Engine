#pragma once

#include <expected>

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
    std::vector<std::shared_ptr<Layer>> m_Layers;
    
    uint32_t SceneID = -1;
public:
    SceneManager* m_SceneManager;
    CameraManager m_CameraManager;
    entt::registry registry;

    Logger logger = Logger("SCENE");
    /**
 * @brief Constructs a Scene with default-initialized members.
 *
 * Initializes an empty scene: no layers, a default CameraManager, a fresh entity registry,
 * and other members set to their declared default values.
 */
Scene() = default;
    /**
 * @brief Default destructor for Scene.
 *
 * Performs any required cleanup when a Scene instance is destroyed.
 */
~Scene() = default;


    virtual void AddLayer(std::shared_ptr<Layer> layer);
    virtual void RemoveLayer(std::shared_ptr<Layer> layer);
    virtual void Draw();
    virtual void Update(float dt);
    virtual void OnEvent(Event& e);
    virtual void OnAttach(SceneManager* sceneManager, uint32_t id); // On attach to the scene manager
    virtual void OnDetach(); // on detach from the scene manager
    
    // EngineContext* because std::expected cant take reference
    std::expected<EngineContext*, bool> GetContext();
    uint32_t GetID() const;
};