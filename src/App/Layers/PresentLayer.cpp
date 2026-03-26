#include "PresentLayer.hpp"

/**
 * @brief Finalizes the current rendering operation and presents the frame.
 *
 * Invokes the layer's renderer to end the current render pass so the rendered frame can be submitted for display.
 */
void PresentLayer::OnDraw()
{
    if (renderer == nullptr)
        return;
    renderer->End();
}

/**
 * @brief Acquire and store the layer's Renderer instance from the global services container.
 *
 * Retrieves the application's Renderer service and stores its address in the layer's
 * `renderer` member for subsequent rendering operations.
 */