#include <App/App.hpp>

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
    
    glfwSetFramebufferSizeCallback(m_EngineContext.ActiveWindow, [](GLFWwindow* window, int width, int height){
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            WindowResizeEvent event(width, height);
            app->OnEvent(event);
        }
    });

    glfwSetKeyCallback(m_EngineContext.ActiveWindow, [](
        GLFWwindow* window, 
        int key, 
        int scancode, 
        int action, 
        int mods) {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            KeyInputEvent event(window, key, scancode, action, mods);
            app->OnEvent(event);
        }
    });

    glfwSetCursorPosCallback(m_EngineContext.ActiveWindow, [](GLFWwindow* window, double xpos, double ypos) {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            MouseMoveEvent event(window, xpos, ypos);
            app->OnEvent(event);
        }
    });
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logger.LogError("Failed to load glad!");
        Shutdown();
        return;
    }
    
    enableReportGlErrors();
	glEnable(GL_DEPTH_TEST); // ensures correct depth rendering
	glEnable(GL_BLEND); // Enables blending
	glDepthFunc(GL_LESS); // default: pass if fragment is closer
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Blending alpha thingy. Basically lets stuff be opaque or not
}
void App::Run()
{
    renderer = std::make_unique<Renderer>(m_EngineContext);
    m_EngineContext.renderer = renderer.get();

    m_SceneManager = std::make_unique<SceneManager>(m_EngineContext);

    auto gameLayer = std::make_shared<GameLayer>();
    auto scene = std::make_shared<Scene>();
    auto scene2 = std::make_shared<Scene>();
    m_SceneManager->AddScene(scene);

    scene->AddLayer(gameLayer);
    float dt;
    auto then = glfwGetTime();
    while (!glfwWindowShouldClose(m_EngineContext.ActiveWindow) && !m_EngineContext.SafeShutdown)
    {
        float now = glfwGetTime();
        dt = now - then;
        then = now;
        glClearColor(0.2f, 0.3f, 0.4f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        renderer->Begin();
        
        // Updating
        renderer->Update(dt);
        m_SceneManager->Update(dt);

        // Drawing
        m_SceneManager->Draw();
        renderer->End();

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

void App::OnEvent(Event& e)
{
    renderer->OnEvent(e);
    m_SceneManager->OnEvent(e);
    m_InputSystem.OnEvent(e);
}