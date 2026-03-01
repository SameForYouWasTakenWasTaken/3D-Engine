#pragma once

#include <vector>
#include <entt.hpp>

#include <Logger.hpp>
#include <Components/Camera.hpp>
#include <Components/Transform.hpp>
#include <Contexts/EngineContext.hpp>

class CameraManager final
{  
    EngineContext& m_EngineContext;
    entt::entity m_ActiveCamera;
public:
    Logger logger = Logger("CAMERA_MANAGER");
    CameraManager(EngineContext& context) : m_EngineContext(context) {};
    ~CameraManager() = default;

    void Update(entt::registry& registry, float dt);
    void SetActiveCamera(entt::entity entity);
    entt::entity GetActiveCamera() const;
}; 