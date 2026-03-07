#include <Engine/Systems/CameraManager.hpp>
#include <Contexts/EngineContext.hpp>

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
        transform->position,
        transform->position + camera->GetForward(),
        camera->GetUp()
    );

    auto fwd = camera->GetForward();

    std::cout << "Camera Rot: "
          << transform->rotation.x << ", "
          << transform->rotation.y << ", "
          << transform->rotation.z << std::endl;
}

void CameraManager::SetActiveCamera(entt::entity entity)
{
    m_ActiveCamera = entity;
}

entt::entity CameraManager::GetActiveCamera() const
{
    return m_ActiveCamera;
}