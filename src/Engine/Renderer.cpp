#include <Engine/Renderer.hpp>

#include "../../cmake-build-debug/_deps/assimp-src/contrib/pugixml/src/pugixml.hpp"

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
}

void Renderer::RenderSceneToFBO()
{
    auto& services = Services::Get();
    auto& materialManager = services.GetService<MaterialManager>();
    auto& textureManager = services.GetService<Texture2DManager>();
    auto& shaderManager = services.GetService<ShaderManager>();

    m_SceneFBO.Bind();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

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

            // Assign specular texture to the material to prevent repeat of this if statement
            material->specular = id.value();
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

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    screenShader->UseProgram();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SceneFBO.GetColorTexture());
    screenShader->SetInt("u_ScreenTexture", 0);

    m_ScreenVAO.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
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
