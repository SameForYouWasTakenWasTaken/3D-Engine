#include <Engine/Systems/CameraManager.hpp>
#include <Contexts/EngineContext.hpp>

/**
 * @brief Update the active camera's projection and view matrices for the current frame.
 *
 * Recomputes the projection matrix if the camera's dirty flag is set and updates the view
 * matrix from the active entity's transform using the camera's forward and up vectors.
 * If no active camera or no transform component is present, emits a warning and returns
 * without modifying camera data.
 *
 * @param registry The entt registry containing camera and transform components.
 * @param dt Frame delta time in seconds.
 */
void CameraManager::Update(entt::registry& registry, float dt)
{
    auto* camera = registry.try_get<COMPCamera>(m_ActiveCamera);
    if (camera == nullptr)
    {
        logger.AppendLogTag("CAMERA_MANAGER", LogColors::YELLOW);
        logger.LogWarning("No active camera set, skipping camera update.");
        logger.DumpLogs();
        return;
    }

    if (std::isnan(camera->GetCameraContext().ratio))
        return;


    CameraContext context = camera->GetCameraContext();

    // Update the projection matrix
    if (camera->dirty_camera)
    {
        camera->projection = glm::perspective(
            glm::radians(context.fov), 
            context.ratio, 
            context.near, 
            context.far
        );
        camera->dirty_camera = false;
    }

    // View matrix
    auto* transform = registry.try_get<COMPTransform>(m_ActiveCamera);
    if (transform == nullptr)
    {
        logger.AppendLogTag("CAMERA_MANAGER", LogColors::YELLOW);
        logger.LogWarning("Active camera has no transform component, skipping view matrix update.");
        logger.DumpLogs();
        return;
    }

    // Build view matrix
    camera->view = glm::lookAt(
        transform->LocalPosition,
        transform->LocalPosition + camera->GetForward(),
        camera->GetUp()
    );
}

/**
 * @brief Set the currently active camera entity for the manager.
 *
 * @param entity The entt::entity that should become the active camera. Passing `entt::null`
 * clears the active camera.
 */
void CameraManager::SetActiveCamera(entt::entity entity)
{
    m_ActiveCamera = entity;
}

entt::entity CameraManager::GetActiveCamera() const
{
    return m_ActiveCamera;
}