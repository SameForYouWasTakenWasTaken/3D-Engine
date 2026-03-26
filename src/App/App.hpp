#pragma once
#include <../Core/Engine/EngineSettings.hpp>
#include <Contexts/EngineContext.hpp>
#include "vendor/glad.h"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "Engine/Renderer/API/VAO.hpp"
#include "Engine/Renderer/API/VBO.hpp"
#include "Engine/Renderer/API/EBO.hpp"
#include "Engine/Renderer/API/Shader.hpp"
#include "Engine/Renderer/API/Debug.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Layers/GameLayer.hpp"
#include "Engine/Events/Events.hpp"
#include "Engine/Systems/ShaderManager.hpp"
#include "Systems/SceneManager.hpp"
#include "Engine/Systems/InputSystem.hpp"
#include "Engine/Systems/AssetManager.hpp"
#include "Engine/Systems/Services.hpp"

#include "Engine/Events/KeyInputEvent.hpp"
#include "Engine/Events/MouseMoveEvent.hpp"

class App final
{
public:
    App(EngineSettings Settings);

    void Run();
    void Shutdown();
    void OnEvent(Event& e);

    EngineContext m_EngineContext;
};