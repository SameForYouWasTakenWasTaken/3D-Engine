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
    CameraManager() = default;
    CameraManager(const CameraManager&) = delete;
    ~CameraManager() = default;

    void Update(entt::registry& registry, float dt);
    void SetActiveCamera(entt::entity entity);
    [[nodiscard]] entt::entity GetActiveCamera() const;
}; 