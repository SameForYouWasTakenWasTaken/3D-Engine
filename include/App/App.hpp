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

class App final
{
    std::unique_ptr<Renderer> renderer;
public:
    App(AppSettings Settings);

    void Run();
    void Shutdown();
    void OnEvent(Event& e);

    EngineContext m_EngineContext;
};