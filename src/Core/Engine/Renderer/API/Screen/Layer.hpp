#pragma once


#include "Scene.hpp"
#include "Engine/Events/Events.hpp"
#include "vendor/Logger.hpp"

class Scene;
class Layer
{
protected:
    Scene* m_Scene = nullptr;

public:
    Logger logger = Logger("LAYER_BASE");
    Layer() = default;
    virtual ~Layer() = default;

    void SetScene(Scene* scene);
    void EmitEvent(Event& e);

    /**
 * @brief Performs the layer's rendering for a frame.
 *
 * Override to implement layer-specific drawing. The default implementation does nothing.
 */
    virtual void OnDraw()
    {
    }

    /**
 * @brief Called each frame to update the layer's state.
 *
 * Implementations should update time-dependent logic or animations using the elapsed time since the previous update.
 *
 * @param dt Time elapsed since the last update in seconds.
 */
    virtual void OnUpdate(float dt)
    {
    }
    /**
 * @brief Called when the layer is attached to its scene or layer stack.
 *
 * Override to perform initialization, allocate resources, or run setup that depends on the associated Scene.
 */
    virtual void OnAttach()
    {
    }

    /**
 * @brief Called when the layer is detached from its Scene or layer stack.
 *
 * Default implementation performs no action; override to respond to detachment
 * (e.g., free resources or unregister callbacks).
 */
    virtual void OnDetach()
    {
    }
};
