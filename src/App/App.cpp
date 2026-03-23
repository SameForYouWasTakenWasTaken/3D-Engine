#include <App/App.hpp>

#include "Engine/Layers/PresentLayer.hpp"

/**
 * @brief Initialize the application context, create the window, and configure OpenGL and input callbacks.
 *
 * Initializes engine window dimensions and logger, starts GLFW, creates the primary GLFW window, binds
 * this App instance to the window, registers framebuffer-size, key and cursor-position callbacks that
 * translate platform events into engine events dispatched via OnEvent, loads OpenGL functions via glad,
 * and configures basic GL state (error reporting, depth testing, blending, depth and blend functions).
 *
 * On failure to initialize GLFW, create the window, or load GL function pointers, an error is logged
 * and Shutdown() is called; the constructor will return without a fully initialized rendering context.
 *
 * @param Settings Application settings providing Width, Height, and Name for the window.
 */
App::App(AppSettings Settings)
{
    m_EngineContext.WindowHeight = Settings.Height;
    m_EngineContext.WindowWidth = Settings.Width;
    auto logger = m_EngineContext.logger;
    logger.RenameLogger("APP");

    if (!glfwInit())
    {
        logger.LogError("Couldn't initialize GLFW!");
        Shutdown();
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_EngineContext.ActiveWindow = glfwCreateWindow(
        m_EngineContext.WindowWidth, m_EngineContext.WindowHeight, // Width, height
        Settings.Name.c_str(), // App name
        NULL, NULL
    );

    if (!m_EngineContext.ActiveWindow)
    {
        logger.LogError("Active window does not exist!");
        Shutdown();
        return;
    }
    glfwSetWindowUserPointer(m_EngineContext.ActiveWindow, this);
    glfwMakeContextCurrent(m_EngineContext.ActiveWindow);

    glfwSetFramebufferSizeCallback(m_EngineContext.ActiveWindow, [](GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            WindowResizeEvent event(window, width, height);
            app->OnEvent(event);
        }
    });

    glfwSetKeyCallback(m_EngineContext.ActiveWindow, [](
                       GLFWwindow* window,
                       int key,
                       int scancode,
                       int action,
                       int mods)
                       {
                           auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
                           if (app)
                           {
                               KeyInputEvent event(window, key, scancode, action, mods);
                               app->OnEvent(event);
                           }
                       });

    glfwSetCursorPosCallback(m_EngineContext.ActiveWindow, [](GLFWwindow* window, double xpos, double ypos)
    {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            MouseMoveEvent event(window, xpos, ypos);
            app->OnEvent(event);
        }
    });

    glfwSetKeyCallback(m_EngineContext.ActiveWindow, [](
                       GLFWwindow* window,
                       int key,
                       int scancode,
                       int action,
                       int mods)
                       {
                           auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
                           if (app)
                           {
                               KeyInputEvent event(window, key, scancode, action, mods);
                               app->OnEvent(event);
                           }
                       });

    glfwSetCursorPosCallback(m_EngineContext.ActiveWindow, [](GLFWwindow* window, double xpos, double ypos)
    {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            MouseMoveEvent event(window, xpos, ypos);
            app->OnEvent(event);
        }
    });

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        logger.LogError("Failed to load glad!");
        Shutdown();
        return;
    }

    enableReportGlErrors();
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST); // ensures correct depth rendering
    glEnable(GL_BLEND); // Enables blending
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS); // default: pass if fragment is closer
    glDepthMask(GL_TRUE); // draw transparent stuff
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Blending alpha thingy. Basically lets stuff be opaque or not

    Services& services = Services::Get();

    services.RegisterService<ShaderManager>(); // Renderer depends on ShaderManager
    services.RegisterService<Renderer>(m_EngineContext);
    services.RegisterService<SceneManager>(m_EngineContext);
    services.RegisterService<InputSystem>();
    services.RegisterService<MaterialManager>();
    services.RegisterService<Texture2DManager>();
    services.RegisterService<AssetManager>();
}

/**
 * @brief Starts and runs the application's main loop, managing rendering and scene updates.
 *
 * Initializes the renderer and scene manager, prepares initial scenes and layers, then enters
 * the main loop which: computes frame delta time, clears the frame, drives the renderer and
 * scene manager update/draw phases each frame, swaps window buffers, and polls events.
 * The loop continues until the GLFW window is flagged to close or the engine context requests
 * a safe shutdown. Calls Shutdown() after exiting the loop.
 */
void App::Run()
{

    auto& services = Services::Get();
    auto& sceneManager = services.GetService<SceneManager>();
    auto& renderer = services.GetService<Renderer>();

    auto gameLayer = std::make_shared<GameLayer>();
    auto presentLayer = std::make_shared<PresentLayer>();

    auto scene = std::make_shared<Scene>();
    sceneManager.AddScene(scene);

    scene->AddLayer(gameLayer);
    scene->AddLayer(presentLayer); // Any layer above will be presented to the screen
    // TODO: add ImGUI

    float dt{};
    auto then = glfwGetTime();

    while (!glfwWindowShouldClose(m_EngineContext.ActiveWindow) && !m_EngineContext.SafeShutdown)
    {
        float now = glfwGetTime();
        dt = now - then;
        then = now;

        renderer.Begin(); // End() runs at PresentLayer
        sceneManager.Update(dt);
        sceneManager.Draw();

        glfwSwapBuffers(m_EngineContext.ActiveWindow);
        glfwPollEvents();
    }

    Shutdown();
}

void App::Shutdown()
{
    // After shutdown
    //m_EngineContext.logger.CreateLogFile(); // TODO: Auto delete logs that are a week old
    glfwTerminate();
}

/**
 * @brief Dispatches an incoming event to the renderer, scene manager, and input system.
 *
 * Forwards the provided event to each subsystem so they can handle input, window, and scene-related events.
 *
 * @param e The event to dispatch to subsystems.
 */
void App::OnEvent(Event& e)
{
    auto& services = Services::Get();

    auto& renderer = services.GetService<Renderer>();
    auto& sceneManager = services.GetService<SceneManager>();
    auto& input = services.GetService<InputSystem>();

    renderer.OnEvent(e);
    sceneManager.OnEvent(e);
    input.OnEvent(e);
}