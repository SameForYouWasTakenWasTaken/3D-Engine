#pragma once

#include <vector>
#include <entt.hpp>

#include <Logger.hpp>
#include <Components/Camera.hpp>
#include <Components/Transform.hpp>

class CameraManager final
{  
    entt::entity m_ActiveCamera;
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
}; 