#pragma once
#include <vendor/entt.hpp>

#include "Engine/Physics/PhysicsWorld.hpp"
#include "Engine/Systems/LightManager.hpp"
#include "Engine/Systems/CameraManager.hpp"

struct SceneContext
{
    SceneContext() = default;
    SceneContext(const SceneContext&) = delete;
    SceneContext& operator=(const SceneContext&) = delete;
    SceneContext(SceneContext&&) = delete;
    SceneContext& operator=(SceneContext&&) = delete;

    CameraManager m_CameraManager;
    entt::registry registry;

    LightManager m_LightManager{registry};
    PhysicsWorld m_PhysicsWorld{registry};
};