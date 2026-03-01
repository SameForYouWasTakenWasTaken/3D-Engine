#include "Engine/Systems/SceneManager.hpp"


void SceneManager::AddScene(std::shared_ptr<Scene> scene)
{
    m_Scenes.push_back(scene);
    if (m_ActiveScene == nullptr)
        m_ActiveScene = scene;
}

void SceneManager::RemoveScene(std::shared_ptr<Scene> scene)
{
    m_Scenes.erase(
        std::remove(m_Scenes.begin(), m_Scenes.end(), scene), 
        m_Scenes.end()
    );
}

void SceneManager::ChangeScene(std::shared_ptr<Scene> scene)
{
    if (std::find(m_Scenes.begin(), m_Scenes.end(), scene) == m_Scenes.end())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogError("Scene not found in scene manager!");
        //logger.DumpLogs();
        return;
    }

    m_ActiveScene = scene;
}

void SceneManager::NextScene()
{
    if (m_Scenes.empty())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogError("No scenes to rotate through!");
        return;
    }

    if (m_ActiveScene == nullptr)
    {
        m_ActiveScene = m_Scenes[0];
        return;
    }

    auto it = std::find(m_Scenes.begin(), m_Scenes.end(), m_ActiveScene);
    if (it == m_Scenes.end())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogError("Active scene not found!");
        return;
    }

    it++; // Move to the next scene
    if (it == m_Scenes.end())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("Active scene is the last scene in the scene manager, cannot go to next scene.");
        return;
    }

    m_ActiveScene = *it;
}
