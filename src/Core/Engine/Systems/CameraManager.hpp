#pragma once

#include <vector>
#include <vendor/entt.hpp>

#include "vendor/Logger.hpp"
#include "Engine/Components/Camera.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/Renderer/API/OpenGL/UBO.hpp"

class CameraManager final
{  
    entt::entity m_ActiveCamera = entt::null;
    UBO cameraUBO;

    // The camera component is too complicated to get
    // this inside. Either way, these are the most important
    // values anyway, so we don't need to add this specific data
    // to the camera component
    struct alignas(16) GPUdata
    {
        glm::mat4 projectmat;
        glm::mat4 viewmat;

        glm::vec3 viewPos;
        float nearPlane;

        float farPlane;
        glm::vec3 _cameraPad;
    };

    GPUdata active_data;
public:
    Logger logger = Logger("CAMERA_MANAGER");
    /**
 * @brief Constructs a CameraManager with its default-initialized state.
 *
 * Initializes the manager so it is ready for use with an entity registry.
 */
CameraManager() = default;
    /**
 * @brief Disables copying of CameraManager instances.
 *
 * Deleting the copy constructor prevents creating a duplicate manager with the same
 * internal state (such as the active camera and logger).
 */
CameraManager(const CameraManager&) = delete;
    /**
 * @brief Destroys the CameraManager.
 */
~CameraManager() = default;

    void Update(entt::registry& registry, float dt);
    void SetActiveCamera(entt::entity entity);
    [[nodiscard]] entt::entity GetActiveCamera() const;
    void UploadGPUData(COMPCamera* cam, COMPTransform* transform);
}; 