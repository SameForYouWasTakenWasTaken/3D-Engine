#include <Engine/Layer.hpp>

/**
 * @brief Dispatches an event to the currently active scene.
 *
 * If no active scene is set, logs an error with tag "LAYER_BASE", dumps logs, and returns without dispatching.
 *
 * @param e Event to forward to the scene's SceneManager.
 */
void Layer::EmitEvent(Event& e)
{
    if (m_Scene == nullptr)
    {
        logger.AppendLogTag("LAYER_BASE", LogColors::MAGENTA);
        logger.LogError("Didn't find an active scene in Layer!");
        logger.DumpLogs();
        return;
    }

    Services::Get().GetService<SceneManager>().OnEvent(e);
}

/**
 * @brief Sets the active scene for this layer.
 *
 * @param scene Pointer to the Scene to assign to the layer; pass `nullptr` to unset the active scene.
 */
void Layer::SetScene(Scene* scene)
{
    m_Scene = scene;
}