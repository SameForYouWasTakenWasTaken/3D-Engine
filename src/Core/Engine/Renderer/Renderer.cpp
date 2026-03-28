#include "Renderer.hpp"

#include "tracy/Tracy.hpp"
#include <tracy/TracyOpenGL.hpp>

#include "Engine/Engine.hpp"
#include "Engine/Events/WindowResizeEvent.hpp"
#include "Engine/Systems/LightManager.hpp"
#include "Engine/Systems/MeshManager.hpp"
#include "Engine/Systems/Texture2DManager.hpp"

/**
 * @brief Constructs a Renderer and initializes renderer state and resources.
 *
 * Initializes internal rendering resources and prepares the renderer to process
 * frames using the provided engine context.
 *
 * @param context Reference to the EngineContext that provides renderer state,
 *                configuration, and access to shared services. The renderer
 *                stores this reference for the lifetime of the renderer.
 */
Renderer::Renderer()
{
    Init();
};

/**
 * @brief Update renderer internal state for the current frame.
 *
 * Advances any time-dependent renderer state (animations, timers, or pending updates).
 *
 * @param dt Time elapsed since the previous update, in seconds.
 */
void Renderer::Update(float dt)
{
}

/**
 * @brief Adds a render submission to the renderer's instancing batches.
 *
 * Builds per-instance transform and normal data from the provided submission and
 * accumulates it into an internal batch keyed by mesh, material ID, and scene.
 * When a batch is created for the first time, the batch's mesh, material ID,
 * scene, and base transform are recorded; subsequent submissions with the same
 * key append instance data to that batch.
 *
 * @param submit Submission containing mesh, materialID, active_scene, and transform used to populate instance data.
 */
void Renderer::Submit(SubmitObject& submit)
{
    // It's a much faster way to batch than comparing the mesh and material pointers directly,
    // and it allows us to easily store batches in an unordered_map
    size_t batchKey =
        std::hash<MeshID>{}(submit.meshID) ^
        (std::hash<MaterialID>{}(submit.materialID) << 1);

    glm::mat4 model = submit.transform->WorldMatrix;
    glm::mat3 normalMat = submit.transform->GetNormalMatrix();

    InstanceData instanceData{};
    instanceData.model = model;
    instanceData.normal0 = glm::vec4(normalMat[0], 0.0f);
    instanceData.normal1 = glm::vec4(normalMat[1], 0.0f);
    instanceData.normal2 = glm::vec4(normalMat[2], 0.0f);


    // prevent unnecessary initialization
    auto [it, inserted] = m_Batches.try_emplace(batchKey);
    auto& batch = it->second;

    // Assign the necessary stuff
    if (inserted)
    {
        batch.meshID = submit.meshID;
        batch.materialID = submit.materialID;
        batch.context = submit.context;
        batch.transform = submit.transform;
    }

    // Send out to the m_Batches unordered_map, which will be used for batch rendering in the End() function
    batch.instances.push_back(instanceData);
}

/**
 * @brief Prepare GPU/state for rendering a single object: configure shader uniforms, ensure textures,
 *        upload lights, and bind the mesh VAO and required textures.
 *
 * Ensures the object's diffuse and specular textures exist (loading and writing default texture IDs
 * back into the material when absent). In wireframe mode, selects a wireframe fragment shader,
 * sets projection/view matrices, binds the mesh VAO, and returns early. Otherwise, sets camera and
 * material uniforms on the object's shader, uploads scene lights, binds diffuse/specular textures to
 * texture slots 0 and 1, and binds the mesh VAO ready for drawing.
 *
 * @param object RenderObject containing mesh, shader, material, textures, and scene context.
 */
bool Renderer::PrepareObject(RenderObject& object)
{
    auto diffuse = object.diffuse;
    auto specular = object.specular;
    auto* shader = object.shader;
    auto* mesh = object.mesh;
    auto* material = object.material;

    auto& SceneContext = object.context;
    auto& lightManager = SceneContext->m_LightManager;
    auto& materialManager = Services::Get().GetService<MaterialManager>();

    entt::entity cam = SceneContext->m_CameraManager.GetActiveCamera();
    auto* TransformCam = SceneContext->registry.try_get<COMPTransform>(cam);
    auto* Camera = SceneContext->registry.try_get<COMPCamera>(cam);


    assert(shader);
    assert(mesh);
    assert(material);

    if (!material || !shader || !mesh || !TransformCam || !Camera) return false;

    CameraContext camContext = Camera->GetCameraContext();

    // Create a default specular texure if none exists
    if (!specular)
    {
        auto& textureManager = Services::Get().GetService<Texture2DManager>();

        TextureSettings textSettings;
        textSettings.Texture_Use_sRGB = false;

        auto id = textureManager.Load(TEXTURES_DIRECTORY "default_spec.png", textSettings);
        specular = textureManager.Get(id.value());

        assert(specular);
        assert(id);

        // Assign specular texture to the material to prevent repeat of this if statement
        material->specular = id.value();
    }

    if (!diffuse)
    {
        auto& textureManager = Services::Get().GetService<Texture2DManager>();
        auto id = textureManager.Load(TEXTURES_DIRECTORY "default_diffuse.png");
        diffuse = textureManager.Get(id.value());

        assert(diffuse);
        assert(id);

        // Assign diffuse texture to the material to prevent repeat of this if statement
        material->diffuse = id.value();
    }

    if (Engine::GetContext().StateCache.Wireframe == OPTION::YES)
    {
        auto WireframeShaderID = Services::Get().GetService<ShaderManager>().Load(
            shader->GetFilepaths().first,
            SHADERS_DIRECTORY "wireframe.frag"
        );
        auto WireframeShader = Services::Get().GetService<ShaderManager>().Get(WireframeShaderID);

        assert(WireframeShader);

        WireframeShader->UseProgram();
        object.context->m_CameraManager.UploadGPUData(Camera, TransformCam);
        mesh->vao.Bind();
        return true;
    }

    shader->UseProgram();
    shader->SetInt("materialDiffuse", 0); // Texture slot 0, activated on Texture->Use() below
    shader->SetInt("materialSpecular", 1); // Slot 1

    lightManager.UploadGPUData(shader, mesh);
    materialManager.UploadToGPU(material);
    object.context->m_CameraManager.UploadGPUData(Camera, TransformCam);

    diffuse->Use(GL_TEXTURE0);
    specular->Use(GL_TEXTURE1);
    mesh->vao.Bind();

    return true;
}

/**
 * @brief Issues an instanced draw for the provided render object and updates render statistics.
 *
 * If wireframe mode is enabled in the engine state, the object is rendered using line polygon mode
 * for the duration of this draw call.
 *
 * @param object RenderObject whose mesh (and bound VAO/state) will be drawn; must have a valid `mesh`.
 * @param InstanceCount Number of instances to render.
 *
 * @details
 * Performs either an indexed instanced draw or a non-indexed instanced draw depending on `mesh->Indexed`.
 * Updates global render statistics (draw calls, vertex/triangle counts and, for indexed draws, indices submitted).
 */
void Renderer::DrawObject(RenderObject& object, size_t InstanceCount)
{
    auto& engineContext = Engine::GetContext();
    if (engineContext.StateCache.Wireframe == OPTION::YES)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    auto* mesh = object.mesh;
    if (mesh->Indexed)
    {
        glDrawElementsInstanced(
            mesh->Primitive,
            mesh->IndexCount,
            GL_UNSIGNED_INT,
            nullptr,
            static_cast<GLsizei>(InstanceCount)
        );
        g_RenderStats.indicesSubmitted += static_cast<uint64_t>(mesh->IndexCount) * InstanceCount;
        g_RenderStats.drawCalls++;
    }
    else
    {
        glDrawArraysInstanced(
            mesh->Primitive,
            0,
            mesh->VertexCount,
            InstanceCount
        );
        g_RenderStats.drawCalls++;
    }

    if (engineContext.StateCache.Wireframe == OPTION::YES)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    g_RenderStats.verticesSubmitted += static_cast<uint64_t>(mesh->VertexCount) * InstanceCount;
    if (mesh->Primitive)
    {
        const auto trianglesPerInstance = mesh->Indexed ? (mesh->IndexCount / 3) : (mesh->VertexCount / 3);
        g_RenderStats.trianglesSubmitted += static_cast<uint64_t>(trianglesPerInstance) * InstanceCount;
    }
}

/**
 * @brief Applies the engine's cached rendering state to OpenGL and the windowing system.
 *
 * @details Compares the current GL/state cache against the engine's pending StateCache and applies only the differences. The function updates depth testing (enable/disable and depth function), depth write mask, stencil test, color blending, face culling (enable/disable, cull mode, front-face winding), polygon mode (wireframe vs fill), and vertical sync (VSync via glfwSwapInterval).
 */
void Renderer::ApplyState()
{
    static GLStateCache state =
    {
        .VSync = OPTION::NONE,
        .Wireframe = OPTION::NONE,

        .DepthTest = OPTION::NONE,
        .DepthFunc = DEPTH_FUNC::NONE,
        .DepthWrite = DEPTH_WRITE::NONE,

        .StencilTest = OPTION::NONE,
        .ColorBlend = OPTION::NONE,

        .Culling = OPTION::NONE,
        .CullingMode = CULLING_MODE::NONE,
        .WindingDir = WINDING_DIR::NONE
    };

    auto& pending = Engine::GetContext().StateCache;

    // --- Depth Test Enable ---
    if (state.DepthTest != pending.DepthTest)
    {
        state.DepthTest = pending.DepthTest;

        if (pending.DepthTest == OPTION::YES)
            glEnable(GL_DEPTH_TEST);
        else if (pending.DepthTest == OPTION::NO)
            glDisable(GL_DEPTH_TEST);
    }

    // --- Depth Func ---
    if (pending.DepthTest == OPTION::YES && state.DepthFunc != pending.DepthFunc)
    {
        state.DepthFunc = pending.DepthFunc;
        glDepthFunc(static_cast<GLenum>(pending.DepthFunc));
    }

    // --- Depth Write ---
    if (state.DepthWrite != pending.DepthWrite)
    {
        state.DepthWrite = pending.DepthWrite;
        glDepthMask(
            pending.DepthWrite == DEPTH_WRITE::ENABLED
                ? GL_TRUE
                : GL_FALSE
        );
    }

    // --- Stencil Test ---
    if (state.StencilTest != pending.StencilTest)
    {
        state.StencilTest = pending.StencilTest;

        if (pending.StencilTest == OPTION::YES)
            glEnable(GL_STENCIL_TEST);
        else if (pending.StencilTest == OPTION::NO)
            glDisable(GL_STENCIL_TEST);
    }

    // --- Blending ---
    if (state.ColorBlend != pending.ColorBlend)
    {
        state.ColorBlend = pending.ColorBlend;

        if (pending.ColorBlend == OPTION::YES)
            glEnable(GL_BLEND);
        else if (pending.ColorBlend == OPTION::NO)
            glDisable(GL_BLEND);
    }

    // --- Face Culling Enable ---
    if (state.Culling != pending.Culling)
    {
        state.Culling = pending.Culling;

        if (pending.Culling == OPTION::YES)
            glEnable(GL_CULL_FACE);
        else if (pending.Culling == OPTION::NO)
            glDisable(GL_CULL_FACE);
    }

    // --- Cull Mode ---
    if (pending.Culling == OPTION::YES && state.CullingMode != pending.CullingMode)
    {
        state.CullingMode = pending.CullingMode;
        glCullFace(ToGLCullMode(pending.CullingMode));
    }

    // --- Front Face Winding ---
    if (pending.Culling == OPTION::YES && state.WindingDir != pending.WindingDir)
    {
        state.WindingDir = pending.WindingDir;
        glFrontFace(ToGLCullDir(pending.WindingDir));
    }

    // --- Wireframe ---
    if (state.Wireframe != pending.Wireframe)
    {
        state.Wireframe = pending.Wireframe;
        glLineWidth(2.0f);
        glPolygonMode(
            GL_FRONT_AND_BACK,
            pending.Wireframe == OPTION::YES ? GL_LINE : GL_FILL
        );
    }

    // --- VSync ---
    if (state.VSync != pending.VSync)
    {
        state.VSync = pending.VSync;

        if (pending.VSync == OPTION::YES)
            glfwSwapInterval(1);
        else if (pending.VSync == OPTION::NO)
            glfwSwapInterval(0);
    }
}

/**
 * @brief Renders all collected batches into the scene framebuffer object.
 *
 * Applies the current GL state, clears the scene FBO, and for each non-empty batch:
 * validates and resolves mesh, material, shader, and textures (loading and assigning
 * default diffuse/specular textures when missing), uploads per-instance data to the
 * mesh instance VBO, calls PrepareObject and DrawObject to perform instanced rendering,
 * and unbinds bound textures. Unbinds the scene FBO on completion.
 */
void Renderer::RenderSceneToFBO()
{
    auto& services = Services::Get();
    auto& materialManager = services.GetService<MaterialManager>();
    auto& textureManager = services.GetService<Texture2DManager>();
    auto& shaderManager = services.GetService<ShaderManager>();
    auto& meshManager = services.GetService<MeshManager>();

    m_SceneFBO.Bind();
    ApplyState();

    glClearColor(0.f, 0.502f, 0.502f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    for (auto& [batchKey, batch] : m_Batches)
    {
        if (batch.instances.empty())
            continue;

        Mesh* mesh = meshManager.Get(batch.meshID);
        Material* material = materialManager.Get(batch.materialID);

        if (mesh == nullptr || material == nullptr)
            continue;

        auto shader = shaderManager.Get(material->shader);
        auto diffuseTexture = textureManager.Get(material->diffuse);
        auto specularTexture = textureManager.Get(material->specular);


        RenderObject Object{};
        Object.mesh = mesh;
        Object.shader = shader.get();
        Object.material = material;
        Object.diffuse = diffuseTexture;
        Object.specular = specularTexture;
        Object.transform = batch.transform;
        Object.context = batch.context;

        bool prepared = PrepareObject(Object);
        if (!prepared) continue;

        mesh->instanceVBO.Bind();
        size_t instanceCount = batch.instances.size();
        assert(instanceCount <= MAX_INSTANCES);

        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            sizeof(InstanceData) * instanceCount,
            batch.instances.data()
        );

        DrawObject(Object, instanceCount);
        Texture2D::Unbind(0);
        Texture2D::Unbind(1);
    }

    FBO::Unbind();

}

void Renderer::PresentTexture(GLuint texture)
{
    auto& engineContext = Engine::GetContext();
    auto saved_depth = engineContext.StateCache.DepthTest;
    auto saved_stencil = engineContext.StateCache.StencilTest;
    engineContext.StateCache.DepthTest = OPTION::NO;
    engineContext.StateCache.StencilTest = OPTION::NO;

    ApplyState();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    m_PreprocessShader->UseProgram();
    m_PreprocessShader->SetInt("u_ScreenTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    m_ScreenVAO.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    engineContext.StateCache.DepthTest = saved_depth;
    engineContext.StateCache.StencilTest = saved_stencil;
    ApplyState();
}

GLuint Renderer::RunPostProcessChain(GLuint inputTexture)
{
    auto& engineContext = Engine::GetContext();

    OPTION savedDepth = engineContext.StateCache.DepthTest;
    OPTION savedStencil = engineContext.StateCache.StencilTest;

    engineContext.StateCache.DepthTest = OPTION::NO;
    engineContext.StateCache.StencilTest = OPTION::NO;
    ApplyState();

    bool usePing = true;
    GLuint currentTexture = inputTexture;

    for (const auto& pass : m_Passes)
    {
        if (!pass.shader) continue;

        // Use Specific anti aliasing?
        if (pass.shader == m_FXAAShader.get())
            if (!engineContext.Config.UseFXAA)
                continue;

        FBO& target = usePing ? m_Ping : m_Pong;
        target.Bind();

        glClear(GL_COLOR_BUFFER_BIT);

        pass.shader->UseProgram();
        pass.shader->SetInt("u_ScreenTexture", 0);
        pass.shader->SetVec2("u_InvScreenSize",
                                glm::vec2(
                                 1.0f / target.GetWidth(),
                                 1.0f / target.GetHeight())
        );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTexture);

        m_ScreenVAO.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        currentTexture = target.GetColorTexture();
        usePing = !usePing;
    }

    FBO::Unbind();

    engineContext.StateCache.DepthTest = savedDepth;
    engineContext.StateCache.StencilTest = savedStencil;
    ApplyState();

    return currentTexture;
}


/**
 * @brief Initializes renderer resources required for offscreen rendering and screen blit.
 *
 * Sets up the scene framebuffer (m_SceneFBO), creates and configures the fullscreen quad
 * geometry (m_ScreenVAO / m_ScreenVBO / m_ScreenEBO) with position and UV vertex attributes,
 * and loads the preprocessing shader used to present the FBO to the screen.
 */
void Renderer::Init()
{
    auto& shaderManager = Services::Get().GetService<ShaderManager>();
    std::vector<GLfloat> vertices =
    {
        // pos      // uv
        -1.f, -1.f, 0.f, 0.f,
        1.f, -1.f, 1.f, 0.f,
        1.f, 1.f, 1.f, 1.f,
        -1.f, 1.f, 0.f, 1.f
    };

    std::vector<GLuint> indices =
    {
        0, 1, 2,
        2, 3, 0
    };
    auto& engineContext = Engine::GetContext();
    m_SceneFBO.Create(engineContext.WindowWidth, engineContext.WindowHeight);
    m_Ping.Create(engineContext.WindowWidth, engineContext.WindowHeight);
    m_Pong.Create(engineContext.WindowWidth, engineContext.WindowHeight);
    m_ScreenVAO.Bind();

    m_ScreenVBO.Bind();
    m_ScreenVBO.SetData(vertices);

    m_ScreenEBO.Bind();
    m_ScreenEBO.SetData(indices);

    AttributePointer a1;
    a1.index = 0;
    a1.size = 2;
    a1.type = GL_FLOAT;
    a1.normalized = GL_FALSE;
    a1.stride = 4 * sizeof(float);
    a1.offset = (void*)0;

    AttributePointer a2;
    a2.index = 1;
    a2.size = 2;
    a2.type = GL_FLOAT;
    a2.normalized = GL_FALSE;
    a2.stride = 4 * sizeof(float);
    a2.offset = (void*)(2 * sizeof(float));

    m_ScreenVAO.LinkAttrib(a1);
    m_ScreenVAO.LinkAttrib(a2);

    auto PreprocessShaderID = shaderManager.Load(
        SHADERS_DIRECTORY "Preprocessing/preprocess.vert",
        SHADERS_DIRECTORY "Preprocessing/preprocess.frag");

    auto FXAAShaderID = shaderManager.Load(
        SHADERS_DIRECTORY "Preprocessing/FXAA.vert",
        SHADERS_DIRECTORY "Preprocessing/FXAA.frag"
    );

    m_PreprocessShader = shaderManager.Get(PreprocessShaderID);
    m_FXAAShader = shaderManager.Get(FXAAShaderID);

    m_Passes.push_back({m_FXAAShader.get()});

    engineContext.EventDispatcher.sink<WindowResizeEvent>().connect<&Renderer::OnResize>(this);
}

/**
 * @brief Prepare the renderer for a new frame by clearing pending batches and resetting statistics.
 *
 * Clears all submitted draw batches from the previous frame and resets global render statistics.
 */
void Renderer::Begin()
{
    ZoneScoped;
    // Clear batches from the last frame
    m_Batches.clear();
    g_RenderStats.Reset();
}

/**
 * @brief Completes the frame by rendering all accumulated batches into the renderer's scene framebuffer and then presenting that framebuffer's color output to the default framebuffer.
 *
 * Renders batched objects into the offscreen scene FBO and then draws the resulting color texture to the screen to finalize the frame.
 */
void Renderer::End()
{
    RenderSceneToFBO();
    GLuint finalTexture = RunPostProcessChain(m_SceneFBO.GetColorTexture());
    glEnable(GL_FRAMEBUFFER_SRGB);
    PresentTexture(finalTexture);
    glDisable(GL_FRAMEBUFFER_SRGB);
}

/**
 * @brief Handles window resize events to update the renderer's framebuffer and viewport.
 *
 * If `e` is a WindowResizeEvent, resizes the scene framebuffer object to the new width/height,
 * binds the framebuffer, and updates the OpenGL viewport accordingly. Other event types are ignored.
 *
 * @param resize Event to handle; expected to be (or contain) a WindowResizeEvent for resize behavior.
 */
void Renderer::OnResize(const WindowResizeEvent& resize)
{
    m_SceneFBO.Resize(resize.Width, resize.Height);
    m_Ping.Resize(resize.Width, resize.Height);
    m_Pong.Resize(resize.Width, resize.Height);

    FBO::Unbind();
    glViewport(0, 0, resize.Width, resize.Height);
}
