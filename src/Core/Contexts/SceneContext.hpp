#pragma once
#include <vendor/entt.hpp>

#include "Engine/Systems/LightManager.hpp"
#include "Engine/Systems/CameraManager.hpp"

struct SceneContext
{
    LightManager m_LightManager;
    CameraManager m_CameraManager;
    entt::registry registry;
};