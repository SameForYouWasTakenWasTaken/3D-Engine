#pragma once
#include <vendor/entt.hpp>

#include "Engine/Systems/LightManager.hpp"
#include "Engine/Systems/CameraManager.hpp"

struct SceneContext
{
    CameraManager m_CameraManager;
    entt::registry registry;
    LightManager m_LightManager{registry};
};