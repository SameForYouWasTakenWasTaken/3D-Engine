#include <Engine/Renderer.hpp>

Renderer::Renderer(EngineContext& context)
    : m_EngineContext(context)
{
    Init();
};

// TODO: Batch updating
void Renderer::Update(float dt)
{
}

void Renderer::Submit(SubmitObject& submit)
{
    // It's a much faster way to batch than comparing the mesh and material pointers directly,
    // and it allows us to easily store batches in an unordered_map
    size_t batchKey =
        std::hash<Mesh*>{}(submit.mesh) ^
        (std::hash<uint32_t>{}(submit.materialID) << 1) ^
        (std::hash<Scene*>{}(submit.active_scene) << 2);


    glm::mat4 model = submit.transform->GetModelMatrix();
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
        batch.mesh = submit.mesh;
        batch.materialID = submit.materialID;
        batch.active_scene = submit.active_scene;
        batch.transform = submit.transform;
    }

    // Send out to the m_Batches unordered_map, which will be used for batch rendering in the End() function
    batch.instances.push_back(instanceData);
}

void Renderer::PrepareObject(RenderObject& object)
{
    auto diffuse = object.diffuse;
    auto specular = object.specular;
    auto* shader = object.shader;
    auto* mesh = object.mesh;
    auto* material = object.material;
    auto* scene = object.scene;

    std::shared_ptr<LightManager> lightManager = scene->m_LightManager;

    entt::entity cam = scene->m_CameraManager.GetActiveCamera();
    auto* TransformCam = scene->registry.try_get<COMPTransform>(cam);
    auto* Camera = scene->registry.try_get<COMPCamera>(cam);

    if (!material || !shader || !mesh || !TransformCam || !Camera) return;

    CameraContext camContext = Camera->GetCameraContext();

    // Create a default specular texure if none exists
    if (!specular)
    {
        auto& textureManager = Services::Get().GetService<Texture2DManager>();
        auto id = textureManager.Load("Resources/Textures2D/default_spec.png");
        specular = textureManager.Get(id.value());

        if (!specular || !id)
            return; // atp code is trolling and sum went wrong, too tired for this bs

        // Assign specular texture to the material to prevent repeat of this if statement
        material->specular = id.value();
    }

    if (!diffuse)
    {
        auto& textureManager = Services::Get().GetService<Texture2DManager>();
        auto id = textureManager.Load("Resources/Textures2D/default_diffuse.png");
        diffuse = textureManager.Get(id.value());

        if (!diffuse || !id)
            return; // atp code is trolling and sum went wrong, too tired for this bs

        // Assign diffuse texture to the material to prevent repeat of this if statement
        material->diffuse = id.value();
    }

    if (m_EngineContext.StateCache.Wireframe == OPTION::YES)
    {
        auto WireframeShaderID = Services::Get().GetService<ShaderManager>().Load(
            shader->GetFilepaths().first,
            "Shaders/wireframe.frag"
            );
        auto WireframeShader = Services::Get().GetService<ShaderManager>().Get(WireframeShaderID);

        WireframeShader->UseProgram();
        WireframeShader->SetMatrix4("projectmat", 1, glm::value_ptr(Camera->projection));
        WireframeShader->SetMatrix4("viewmat", 1, glm::value_ptr(Camera->view));
        mesh->vao.Bind();
        return;
    }

    shader->UseProgram();
    shader->SetMatrix4("projectmat", 1, glm::value_ptr(Camera->projection));
    shader->SetMatrix4("viewmat", 1, glm::value_ptr(Camera->view));
    shader->SetVec3("cam.viewPos", TransformCam->position);
    shader->SetFloat("cam.near", camContext.near);
    shader->SetFloat("cam.far", camContext.far);

    shader->SetVec3("material.ambient", material->ambient);
    shader->SetInt("material.diffuse", 0); // Texture slot 0, activated on Texture->Use() below
    shader->SetInt("material.specular", 1); // Slot 1
    shader->SetFloat("material.shininess", material->shininess);
    shader->SetFloat("material.transparency", glm::clamp(material->transparency, 0.0f, 1.0f));
    lightManager->UploadToShader(shader);

    diffuse->Use(GL_TEXTURE0);
    specular->Use(GL_TEXTURE1);
    mesh->vao.Bind();
}

void Renderer::DrawObject(RenderObject& object, size_t InstanceCount)
{
    if (m_EngineContext.StateCache.Wireframe == OPTION::YES)
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
        g_RenderStats.indicesSubmitted += mesh->IndexCount;
    }
    else
    {
        glDrawArraysInstanced(
            mesh->Primitive,
            0,
            mesh->VertexCount,
            InstanceCount
        );
    }

    if (m_EngineContext.StateCache.Wireframe == OPTION::YES)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    g_RenderStats.drawCalls++;
    g_RenderStats.verticesSubmitted += mesh->VertexCount;
    g_RenderStats.trianglesSubmitted += mesh->VertexCount / 3;
}

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

    auto& pending = m_EngineContext.StateCache;

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

void Renderer::RenderSceneToFBO()
{
    auto& services = Services::Get();
    auto& materialManager = services.GetService<MaterialManager>();
    auto& textureManager = services.GetService<Texture2DManager>();
    auto& shaderManager = services.GetService<ShaderManager>();

    m_SceneFBO.Bind();

    ApplyState();

    glClearColor(0.f, 0.502f, 0.502f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    for (auto& [batchKey, batch] : m_Batches)
    {
        if (batch.instances.empty())
            continue;

        Mesh* mesh = batch.mesh;
        Material* material = materialManager.Get(batch.materialID);

        if (mesh == nullptr || material == nullptr)
            continue;

        auto shader = shaderManager.Get(material->shader);
        auto diffuseTexture = textureManager.Get(material->diffuse);
        auto specularTexture = textureManager.Get(material->specular);

        if (!shader)
            continue;



        if (!diffuseTexture)
        {
            auto id = textureManager.Load("Resources/Textures2D/default_diffuse.png");
            diffuseTexture = textureManager.Get(id.value());

            if (!diffuseTexture || !id)
                continue; // atp code is trolling and sum went wrong, too tired for this bs

            // Assign diffuse texture to the material to prevent repeat of this if statement
            material->diffuse = id.value();
        }

        if (!specularTexture)
        {
            auto id = textureManager.Load("Resources/Textures2D/default_spec.png");
            specularTexture = textureManager.Get(id.value());

            if (!specularTexture || !id)
                continue; // atp code is trolling and sum went wrong, too tired for this bs

            // Assign specular texture to the material to prevent repeat of this if statement
            material->specular = id.value();
        }

        RenderObject Object{};
        Object.mesh = mesh;
        Object.shader = shader.get();
        Object.material = material;
        Object.diffuse = diffuseTexture;
        Object.specular = specularTexture;
        Object.transform = batch.transform;
        Object.scene = batch.active_scene;

        mesh->instanceVBO.Bind();

        size_t instanceCount = batch.instances.size();

        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            sizeof(InstanceData) * instanceCount,
            batch.instances.data()
        );

        PrepareObject(Object);
        DrawObject(Object, instanceCount);

        diffuseTexture->Unbind();
        specularTexture->Unbind();
    }

    FBO::Unbind();
}

void Renderer::PresentScene()
{
    auto& services = Services::Get();
    auto& shaderManager = services.GetService<ShaderManager>();

    auto screenShader = shaderManager.Get(m_PreprocessShaderID);
    if (!screenShader)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OPTION SavedOption = m_EngineContext.StateCache.DepthTest;
    m_EngineContext.StateCache.DepthTest = OPTION::NO; // Disable depth testing for this

    ApplyState();

    glClear(GL_COLOR_BUFFER_BIT);

    screenShader->UseProgram();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SceneFBO.GetColorTexture());
    screenShader->SetInt("u_ScreenTexture", 0);

    m_ScreenVAO.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    m_EngineContext.StateCache.DepthTest = SavedOption; //Reset back to the previously saved option
}

void Renderer::Init()
{
    auto& shaderManager = Services::Get().GetService<ShaderManager>();
    std::vector<GLfloat> vertices =
    {
        // pos      // uv
        -1.f, -1.f, 0.f, 0.f,
         1.f, -1.f, 1.f, 0.f,
         1.f,  1.f, 1.f, 1.f,
        -1.f,  1.f, 0.f, 1.f
    };

    std::vector<GLuint> indices =
    {
        0, 1, 2,
        2, 3, 0
    };

    m_SceneFBO.Create(m_EngineContext.WindowWidth, m_EngineContext.WindowHeight);
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

    m_PreprocessShaderID = shaderManager.Load("Shaders/Preprocessing/preprocess.vert", "Shaders/Preprocessing/preprocess.frag");
}

void Renderer::Begin()
{
    // Clear batches from the last frame
    m_Batches.clear();
    g_RenderStats.Reset();
}

/**
 * @brief Finalizes and flushes all accumulated render batches to the GPU.
 *
 * Iterates stored batches, skips empty ones, binds each batch's mesh VAO and
 * instance VBO, updates the instance buffer with the batch's InstanceData,
 * activates the material's shader, and issues an instanced draw call.
 * For meshes marked as indexed, uses glDrawElementsInstanced with the mesh's
 * index count; for non-indexed meshes, uses glDrawArraysInstanced with the
 * mesh's vertex count.
 */
void Renderer::End()
{
    RenderSceneToFBO();
    PresentScene();
}

void Renderer::OnEvent(Event& e)
{
    if (e.GetType() == WindowResizeEvent::GetStaticType())
    {
        auto& resize = static_cast<WindowResizeEvent&>(e);
        m_SceneFBO.Resize(resize.Width, resize.Height);
        m_SceneFBO.Bind();
        glViewport(0, 0, resize.Width, resize.Height);
    }
}
