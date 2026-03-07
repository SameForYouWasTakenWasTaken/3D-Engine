#include "Engine/Systems/SceneManager.hpp"


void SceneManager::AddScene(std::shared_ptr<Scene> scene)
{
    m_Scenes.push_back(scene);
    scene->OnAttach(this);
    if (m_ActiveScene == nullptr)
        m_ActiveScene = scene;
}

void SceneManager::RemoveScene(std::shared_ptr<Scene> scene)
{
    scene->OnDetach();
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

void SceneManager::OnUpdate(Event& e)
{
    if (!e.Handled && m_EventCallback)
        m_EventCallback(e);
}

void SceneManager::SetEventCallback(const EventCallbackFn& callback)
{
    m_EventCallback = callback;
}

void SceneManager::OnEvent(Event& e)
{
    // Handle via callbacks
    if (!e.Handled && m_EventCallback)
        m_EventCallback(e);
    
    // Still not handled? Maybe the scene needs to handle it.
    if (!e.Handled)
        m_ActiveScene->OnEvent(e);
}

void SceneManager::Draw()
{
    if (m_ActiveScene == nullptr)
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("No active scene set, skipping render.");
        logger.DumpLogs();
        return;
    }

    if (m_EngineContext.renderer == nullptr)
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("No renderer set, skipping render.");
        logger.DumpLogs();
        return;
    }

     m_ActiveScene->Draw();
}

void SceneManager::Update(float dt)
{
    if (m_ActiveScene == nullptr)
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("No active scene set, skipping update.");
        logger.DumpLogs();
        return;
    }
    
    m_ActiveScene->Update(dt);
}