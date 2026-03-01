#pragma once

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

#include <Logger.hpp>




struct EngineContext 
{
    // Window properties
    GLFWwindow* ActiveWindow;
    int Width, Height;
    
    bool Resizable = true; // Window resizability
    bool LockMouse = false; // Lock mouse to its last position

    bool SafeShutdown = false; // If a critical error happens, this should be toggled to safely shutdown
    
    // Logging related stuff
    Logger logger = Logger("EngineContext");
};