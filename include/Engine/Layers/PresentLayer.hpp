#pragma once

#include <Engine/Layer.hpp>

class PresentLayer : public Layer
{
     Renderer* renderer;
public:
    /**
 * @brief Constructs a PresentLayer instance.
 *
 * @note The member `renderer` is not initialized by this constructor and must be set
 *       before use.
 */
PresentLayer() {};
    /**
 * @brief Destroy the PresentLayer and release its resources.
 *
 * Overrides the base Layer destructor to ensure layer-specific cleanup on destruction.
 */
~PresentLayer() override = default;

    void OnDraw() override;
    void OnAttach() override;
    /**
 * @brief Per-frame update hook that performs no action for this layer.
 *
 * @param dt Elapsed time since the last frame, in seconds.
 */
void OnUpdate(float dt) override {}
    /**
 * @brief Receives an event dispatched to the layer.
 *
 * This implementation intentionally ignores all events.
 *
 * @param e Event dispatched to the layer.
 */
void OnEvent(Event& e) override {}
    /**
 * @brief Called when the layer is detached from the engine.
 *
 * Default implementation performs no teardown or resource release.
 */
void OnDetach() override {}
};