#pragma once

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>
#include <entt.hpp>
#include <glm/mat4x4.hpp>
#include <Engine/Renderer.hpp>
#include <Components/Camera.hpp>

#include <Logger.hpp>



class Renderer; // Forward declaration
struct EngineContext 
{
    // Window properties
    GLFWwindow* ActiveWindow = nullptr;
    int WindowWidth, WindowHeight;
    
    bool Resizable = true; // Window resizability

    // Mouse
    bool LockMouse = false; // Lock mouse to its last position

    // Boot
    bool SafeShutdown = false; // If a critical error happens, this should be toggled to safely shutdown
    
    // Events
    entt::dispatcher EventDispatcher;

    // Logging related stuff
    Logger logger = Logger("EngineContext");
};