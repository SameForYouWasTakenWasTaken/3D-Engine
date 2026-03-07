#include "Engine/Systems/SceneManager.hpp"


void SceneManager::AddScene(std::shared_ptr<Scene> scene)
{
    m_Scenes.emplace(m_NextSceneID, scene);
    scene->OnAttach(this, m_NextSceneID);
    
    m_NextSceneID++;
    
    if (m_ActiveScene == nullptr)
        m_ActiveScene = scene;
}

void SceneManager::RemoveScene(uint32_t sceneID)
{
    auto scene = GetSceneFromID(sceneID);
    scene->OnDetach();

    m_Scenes.erase(sceneID);

    m_ActiveScene = nullptr;
}

void SceneManager::ChangeScene(uint32_t sceneID)
{
    auto scene = GetSceneFromID(sceneID);
    if (m_Scenes.find(sceneID) == m_Scenes.end())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogError("Scene not found in scene manager!");
        logger.DumpLogs();
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
        auto element = *m_Scenes.begin();
        m_ActiveScene = element.second;
        return;
    }

    auto it = m_Scenes.find(m_ActiveScene->GetID());

    it++; // Move to the next scene
    if (it == m_Scenes.end())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("Active scene is either the last scene in the scene manager or it does not exist, cannot go to next scene.");
        return;
    }

    m_ActiveScene = it->second;
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
        static bool warned = false;
        if (warned) return;

        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("No active scene set, skipping render.");
        logger.DumpLogs();

        warned = true;
        return;
    }

    if (m_EngineContext.renderer == nullptr)
    {
        static bool warned = false;
        if (warned) return;

        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("No renderer set, skipping render.");
        logger.DumpLogs();
        warned = true;
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

std::shared_ptr<Scene> SceneManager::GetSceneFromID(uint32_t id)
{
    
    auto it = m_Scenes.find(id);
    return it->second;
}