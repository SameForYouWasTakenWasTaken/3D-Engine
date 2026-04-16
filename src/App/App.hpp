#pragma once
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "Engine/EngineSettings.hpp"

class App final
{
public:
    App(EngineSettings Settings);

    void Run();
    void Shutdown();
};