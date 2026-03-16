#include <Engine/Layer.hpp>

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

void Layer::SetScene(Scene* scene)
{
    m_Scene = scene;
}