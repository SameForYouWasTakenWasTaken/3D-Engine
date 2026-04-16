#include "App.hpp"

#include "Layers/GameLayer.hpp"
#include "Layers/ImGUI_DebugLayer.hpp"
#include "Layers/PresentLayer.hpp"

#include <tracy/Tracy.hpp>

#include "Engine/Engine.hpp"
#include "Systems/SceneManager.hpp"

/**
 * @brief Initialize application window, OpenGL context, input callbacks, and engine services.
 *
 * Sets engine window dimensions, creates the primary GLFW window and binds this App as the window user
 * pointer, registers framebuffer-size, key, and cursor-position callbacks that forward platform events
 * to OnEvent, loads OpenGL function pointers, and configures basic GL state and blending. Also registers
 * core engine services (shader, renderer, scene manager, input, material/texture/asset managers).
 *
 * On failure to initialize GLFW, create the window, or load GL function pointers, Shutdown() is invoked
 * to tear down any partially initialized state and the constructor returns without a fully initialized
 * rendering context.
 *
 * @param Settings Application settings providing Width, Height, and Name for the window.
 */
App::App(EngineSettings Settings)
{
    Engine engine; // Initialize the engine
    engine.Init(Settings);

    const auto& engineContext = Engine::GetContext();
    glfwSetWindowUserPointer(engineContext.ActiveWindow, this);

    glfwSetFramebufferSizeCallback(engineContext.ActiveWindow, [](GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            Engine::GetContext().EventDispatcher.trigger<WindowResizeEvent>(
                WindowResizeEvent(window, width, height)
            );
        }
    });

    glfwSetKeyCallback(engineContext.ActiveWindow, [](
                       GLFWwindow* window,
                       int key,
                       int scancode,
                       int action,
                       int mods)
                       {
                           auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
                           if (app)
                           {
                               Engine::GetContext().EventDispatcher.trigger<KeyInputEvent>(
                                   KeyInputEvent(window, key, scancode, action, mods)
                               );

                               InputSystem::OnEvent(KeyInputEvent{
                                   window, key, scancode, action, mods
                               });
                           }
                       });

    glfwSetCursorPosCallback(engineContext.ActiveWindow, [](GLFWwindow* window, double xpos, double ypos)
    {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            Engine::GetContext().EventDispatcher.trigger<MouseMoveEvent>(
                MouseMoveEvent(window, xpos, ypos)
            );
        }
    });

    Engine::InitServices();
    Services::Get().RegisterService<SceneManager>();
}

/**
 * @brief Starts the application's main loop and drives per-frame updates and rendering.
 *
 * Prepares initial scenes and layers using the SceneManager, then runs the loop until the
 * GLFW window is requested to close or the engine requests a safe shutdown. While running,
 * the loop advances frame time, updates subsystems, and issues rendering commands. Calls
 * Shutdown() after exiting the loop.
 */
void App::Run()
{
    auto& services = Services::Get();
    auto& sceneManager = services.GetService<SceneManager>();
    auto& renderer = services.GetService<Renderer>();
    const auto& engineContext = Engine::GetContext();

    auto scene = std::make_shared<Scene>();
    sceneManager.AddScene(scene);

    scene->PushLayer<GameLayer>();
    scene->PushLayer<PresentLayer>();
    scene->PushLayer<ImGUI_DebugLayer>();

    float dt{};
    auto then = glfwGetTime();
    while (!glfwWindowShouldClose(engineContext.ActiveWindow) && !engineContext.SafeShutdown)
    {
        float now = glfwGetTime();
        dt = now - then;
        then = now;

        renderer.Begin(); // End() runs at PresentLayer
        sceneManager.Update(dt);
        sceneManager.Draw();

        glfwSwapBuffers(engineContext.ActiveWindow);
        glfwPollEvents();

        FrameMark;
    }
    Shutdown();
}

void App::Shutdown()
{
    // After shutdown
    //engineContext.logger.CreateLogFile(); // TODO: Auto delete logs that are a week old
    glfwTerminate();
}