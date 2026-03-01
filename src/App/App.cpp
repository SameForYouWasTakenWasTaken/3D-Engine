#include <App/App.hpp>

App::App(AppSettings Settings)
{
    m_EngineContext.Height = Settings.Height;
    m_EngineContext.Width = Settings.Width;
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
        m_EngineContext.Width, m_EngineContext.Height, // Width, height
        Settings.Name.c_str(), // App name
        NULL, NULL
    );
    
    if (!m_EngineContext.ActiveWindow)
    {
        logger.LogError("Active window does not exist!");
        Shutdown();
        return;
    }
    //glfwSetWindowUserPointer(m_EngineContext.ActiveWindow, this);
    glfwMakeContextCurrent(m_EngineContext.ActiveWindow);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logger.LogError("Failed to load glad!");
        Shutdown();
        return;
    }
    
    enableReportGlErrors();
    glEnable(GL_DEPTH_TEST);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Blending alpha thingy. Basically lets stuff be opaque or not
}
void App::Run()
{
    Renderer renderer(m_EngineContext);
    entt::registry registry;


    auto gameLayer = std::make_shared<GameLayer>();
    auto scene = std::make_shared<Scene>(m_EngineContext);
    scene->AddLayer(gameLayer);

    renderer.m_SceneManager->AddScene(scene);
    

    while (!glfwWindowShouldClose(m_EngineContext.ActiveWindow) && !m_EngineContext.SafeShutdown)
    {
        glClearColor(0.2f, 0.3f, 0.4f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        renderer.Begin();
        renderer.Update(0.016f); // TODO: Get actual delta time
        renderer.End();

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