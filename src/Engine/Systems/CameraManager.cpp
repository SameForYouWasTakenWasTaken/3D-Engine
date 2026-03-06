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

    glm::mat4 view_mat = glm::mat4(1.f);
    view_mat = glm::translate(view_mat, transform->position);
    view_mat = glm::rotate(view_mat, glm::radians(transform->rotation.x), {1,0,0});
    view_mat = glm::rotate(view_mat, glm::radians(transform->rotation.y), {0,1,0});
    view_mat = glm::rotate(view_mat, glm::radians(transform->rotation.z), {0,0,1});

    camera->view = view_mat;
}

void CameraManager::SetActiveCamera(entt::entity entity)
{
    m_ActiveCamera = entity;
}

entt::entity CameraManager::GetActiveCamera() const
{
    return m_ActiveCamera;
}