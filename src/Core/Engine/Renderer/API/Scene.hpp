#pragma once

#include <optional>
#include <memory>
#include "vendor/Logger.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Contexts/SceneContext.hpp"

class Scene final
{
    uint32_t SceneID = -1;
    SceneContext m_SceneContext{};
public:

    Logger logger = Logger("SCENE");
    /**
 * @brief Constructs a Scene with default-initialized members.
 *
 * Initializes an empty scene: no layers, a default CameraManager, a fresh entity registry,
 * and other members set to their declared default values.
 */
    Scene();
    /**
 * @brief Default destructor for Scene.
 *
 * Performs any required cleanup when a Scene instance is destroyed.
 */
    ~Scene() = default;

    virtual void Update(float dt);
    virtual void OnAttach(uint32_t id); // On attach to the scene manager

    // EngineContext* because std::expected cant take reference
    SceneContext& GetContext();
    uint32_t GetID() const;
};

