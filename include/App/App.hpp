#pragma once
#include <App/AppSettings.hpp>
#include <Contexts/EngineContext.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <Engine/LowLevel/VAO.hpp>
#include <Engine/LowLevel/VBO.hpp>
#include <Engine/LowLevel/EBO.hpp>
#include <Engine/LowLevel/Shader.hpp>
#include <App/Debug.hpp>
#include <Engine/Renderer.hpp>
#include <Engine/Layers/GameLayer.hpp>
#include <Engine/Events.hpp>
#include <Engine/Systems/ShaderManager.hpp>
#include <Engine/Systems/SceneManager.hpp>
#include <Engine/Systems/InputSystem.hpp>
#include <App/Services.hpp>

#include <Engine/Events/KeyInputEvent.hpp>
#include <Engine/Events/MouseMoveEvent.hpp>

class App final
{
public:
    App(AppSettings Settings);

    void Run();
    void Shutdown();
    void OnEvent(Event& e);

    EngineContext m_EngineContext;
};