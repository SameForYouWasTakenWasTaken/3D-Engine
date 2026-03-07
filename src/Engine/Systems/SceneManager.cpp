#include "Engine/Systems/SceneManager.hpp"


/**
 * @brief Adds a scene to the manager and assigns it a new numeric ID.
 *
 * The scene is stored in the manager, attached with the assigned ID via the scene's
 * OnAttach method, and the manager's next-scene ID counter is advanced. If there
 * is no currently active scene, the added scene becomes the active scene.
 *
 * @param scene Shared pointer to the scene to add; must be non-null.
 */
void SceneManager::AddScene(std::shared_ptr<Scene> scene)
{
    m_Scenes.emplace(m_NextSceneID, scene);
    scene->OnAttach(this, m_NextSceneID);
    
    m_NextSceneID++;
    
    if (m_ActiveScene == nullptr)
        m_ActiveScene = scene;
}

/**
 * @brief Remove the scene with the given ID, detach it, and clear the active scene.
 *
 * Calls the scene's OnDetach, erases the scene entry from the manager's internal map, and sets the active scene pointer to nullptr.
 *
 * @param sceneID ID of the scene to remove.
 */
void SceneManager::RemoveScene(uint32_t sceneID)
{
    auto scene = GetSceneFromID(sceneID);
    scene->OnDetach();

    m_Scenes.erase(sceneID);

    if (m_ActiveScene == scene)
        m_ActiveScene = nullptr;
}

void SceneManager::ChangeScene(uint32_t sceneID)
{
    auto scene = GetSceneFromID(sceneID);
    if (scene == nullptr)
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogError("Scene not found in scene manager!");
        logger.DumpLogs();
        return;
    }
    if (m_Scenes.find(sceneID) == m_Scenes.end())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogError("Scene not found in scene manager!");
        logger.DumpLogs();
        return;
    }

    m_ActiveScene = scene;
}

/**
 * @brief Advance the active scene to the next scene in the manager's ordering.
 *
 * If there are no scenes, the function logs an error and does nothing.
 * If there is no currently active scene, the first scene in the internal map becomes active.
 * If the current active scene is the last entry or is not found in the map, the function logs a warning and leaves the active scene unchanged.
 */
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
    if (it == m_Scenes.end())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("Active scene is not registered in the scene manager.");
        return;
    }

    ++it; // Move to the next scene

    if (it == m_Scenes.end())
    {
        logger.AppendLogTag("SCENE_MANAGER", LogColors::CYAN);
        logger.LogWarning("Active scene is either the last scene in the scene manager or it does not exist, cannot go to next scene.");
        return;
    }

    m_ActiveScene = it->second;
}

/**
 * @brief Sets the function that will receive incoming events before they are forwarded to the active scene.
 *
 * @param callback Function called for each incoming Event; the callback may mark the event as handled to prevent further processing.
 */
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

/**
 * @brief Renders the currently active scene.
 *
 * If an active scene and a renderer are available, invokes the active scene's Draw method.
 * If there is no active scene or no renderer, emits a one-time warning and skips rendering.
 */
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

/**
 * @brief Updates the currently active scene.
 *
 * If no active scene is set, logs a warning and returns without performing an update.
 *
 * @param dt Time elapsed since the last update, in seconds.
 */
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

/**
 * @brief Retrieves the scene associated with the specified scene ID.
 *
 * @param id Numeric identifier of the scene to retrieve.
 * @return std::shared_ptr<Scene> Shared pointer to the scene corresponding to `id`.
 *         Behavior is undefined if `id` is not present in the manager's scene map.
 */
std::shared_ptr<Scene> SceneManager::GetSceneFromID(uint32_t id)
{
    
    auto it = m_Scenes.find(id);
    return it == m_Scenes.end() ? nullptr : it->second;
}