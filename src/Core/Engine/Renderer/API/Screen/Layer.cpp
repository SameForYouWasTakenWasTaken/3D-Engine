#include "Layer.hpp"

/**
 * @brief Sets the active scene for this layer.
 *
 * @param scene Pointer to the Scene to assign to the layer; pass `nullptr` to unset the active scene.
 */
void Layer::SetScene(Scene* scene)
{
    m_Scene = scene;
}