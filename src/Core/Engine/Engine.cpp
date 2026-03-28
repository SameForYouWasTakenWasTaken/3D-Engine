#include "Engine.hpp"

#include "EngineSettings.hpp"
#include "GLFW/glfw3.h"
#include "Renderer/Renderer.hpp"
#include "Renderer/API/Debug.hpp"
#include "Systems/AssetManager.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/MeshManager.hpp"
#include "Systems/Services.hpp"
#include "Systems/ShaderManager.hpp"
#include "Systems/Texture2DManager.hpp"

EngineContext Engine::m_EngineContext{};

bool Engine::Init(EngineSettings settings)
{
    m_EngineContext.WindowHeight = settings.Height;
    m_EngineContext.WindowWidth = settings.Width;
    auto logger = m_EngineContext.logger;
    logger.RenameLogger("ENGINE");

    if (!glfwInit())
    {
        logger.LogError("Couldn't initialize GLFW!");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4); // Activate MSAA

    m_EngineContext.ActiveWindow = glfwCreateWindow(
        m_EngineContext.WindowWidth, m_EngineContext.WindowHeight, // Width, height
        settings.Name.c_str(), // App name
        NULL, NULL
    );
    glfwMakeContextCurrent(m_EngineContext.ActiveWindow);

    if (!m_EngineContext.ActiveWindow)
    {
        logger.LogError("Active window does not exist!");
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        logger.LogError("Failed to load glad!");
        return false;
    }

    enableReportGlErrors();
    glEnable(GL_MULTISAMPLE); // Enable MSAA
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Blending alpha thingy. Basically lets stuff be opaque or not
    return true;
}

void Engine::InitServices()
{
    Services& services = Services::Get();

    services.RegisterService<ShaderManager>(); // Renderer depends on ShaderManager
    services.RegisterService<Renderer>();
    services.RegisterService<InputSystem>();
    services.RegisterService<MaterialManager>();
    services.RegisterService<Texture2DManager>();
    services.RegisterService<AssetManager>();
    services.RegisterService<MeshManager>();
}
