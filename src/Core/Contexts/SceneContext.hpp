#pragma once
#include <vendor/entt.hpp>

#include "Engine/Physics/PhysicsWorld.hpp"
#include "Engine/Systems/LightManager.hpp"
#include "Engine/Systems/CameraManager.hpp"

struct SceneContext
{
    CameraManager m_CameraManager;
    entt::registry registry;

    LightManager m_LightManager{registry};
    PhysicsWorld m_PhysicsWorld{registry};
};