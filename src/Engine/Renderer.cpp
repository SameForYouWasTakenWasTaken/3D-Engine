#include <Engine/Renderer.hpp>

Renderer::Renderer(EngineContext& context)
    : m_EngineContext(context)
{
};

// TODO: Batch updating
void Renderer::Update(float dt)
{
}

void Renderer::Submit(Mesh* mesh, uint32_t materialID, COMPTransform* transform,
                      Scene* active_scene)
{
    // It's a much faster way to batch than comparing the mesh and material pointers directly,
    // and it allows us to easily store batches in an unordered_map
    size_t batchKey =
        std::hash<Mesh*>{}(mesh) ^
        (std::hash<uint32_t>{}(materialID) << 1) ^
        (std::hash<Scene*>{}(active_scene) << 2);


    glm::mat4 model = transform->GetModelMatrix();
    glm::mat3 normalMat = transform->GetNormalMatrix();

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
        batch.mesh = mesh;
        batch.materialID = materialID;
        batch.active_scene = active_scene;
    }

    // Send out to the m_Batches unordered_map, which will be used for batch rendering in the End() function
    batch.instances.push_back(instanceData);
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
    auto& services = Services::Get();
    auto& materialManager = services.GetService<MaterialManager>();
    auto& textureManager = services.GetService<Texture2DManager>();
    auto& shaderManager = services.GetService<ShaderManager>();

    // Loop through each batch and render it
    for (auto& [batchKey, batch] : m_Batches)
    {
        if (batch.instances.empty())
            continue;

        // Iterate through each mesh. If it's just one, it'll be just one.
        // Primary use is just because models have multiple meshes, so this needs to be done
        Mesh* mesh = batch.mesh;
        Material* material = materialManager.Get(batch.materialID);

        auto shader = shaderManager.Get(material->shader);

        auto diffuseTexture = textureManager.Get(material->diffuse);
        auto specularTexture = textureManager.Get(material->specular);

        std::shared_ptr<LightManager> lightManager = batch.active_scene->m_LightManager;

        entt::entity cam = batch.active_scene->m_CameraManager.GetActiveCamera();
        auto* TransformCam = batch.active_scene->registry.try_get<COMPTransform>(cam);
        auto* Camera = batch.active_scene->registry.try_get<COMPCamera>(cam);

        if (!material || !shader || !mesh || !TransformCam || !Camera) continue;

        CameraContext camContext = Camera->GetCameraContext();

        // Create a default specular texure if none exists
        if (!specularTexture)
        {
            auto id = textureManager.Load("Resources/Textures2D/default_spec.png");
            specularTexture = textureManager.Get(id.value());

            if (!specularTexture || !id)
                continue; // atp code is trolling and sum went wrong, too tired for this bs

            // Assign specular texture to the material to prevent repeat of this if statement
            material->specular = id.value();
        }

        if (!diffuseTexture)
        {
            auto id = textureManager.Load("Resources/Textures2D/default_diffuse.png");
            diffuseTexture = textureManager.Get(id.value());

            if (!diffuseTexture || !id)
                continue; // atp code is trolling and sum went wrong, too tired for this bs

            // Assign diffuse texture to the material to prevent repeat of this if statement
            material->diffuse = id.value();
        }

        mesh->instanceVBO.Bind();

        size_t instanceCount = batch.instances.size();
        // Update buffer data allocated inside the mesh, not reallocate
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            instanceCount * sizeof(InstanceData),
            batch.instances.data()
        );

        shader->UseProgram();
        shader->SetMatrix4("projectmat", 1, glm::value_ptr(Camera->projection));
        shader->SetMatrix4("viewmat", 1, glm::value_ptr(Camera->view));
        shader->SetVec3("cam.viewPos",TransformCam->position);
        shader->SetFloat("cam.near", camContext.near);
        shader->SetFloat("cam.far",camContext.far);

        shader->SetVec3("material.ambient", material->ambient);
        shader->SetInt("material.diffuse", 0); // Texture slot 0, activated on Texture->Use() below
        shader->SetInt("material.specular", 1); // Slot 1
        shader->SetFloat("material.shininess", material->shininess);
        shader->SetFloat("material.transparency", glm::clamp(material->transparency, 0.0f, 1.0f));
        shader->SetFloat("material.transparency", glm::clamp(material->transparency, 0.0f, 1.0f));
        lightManager->UploadToShader(shader.get());

        diffuseTexture->Use(GL_TEXTURE0);
        specularTexture->Use(GL_TEXTURE1);
        mesh->vao.Bind();

        if (mesh->Indexed)
        {
            glDrawElementsInstanced(
                mesh->Primitive,
                mesh->IndexCount,
                GL_UNSIGNED_INT,
                nullptr,
                static_cast<GLsizei>(batch.instances.size())
            );
        }
        else
        {
            glDrawArraysInstanced(
                mesh->Primitive,
                0,
                mesh->VertexCount,
                instanceCount
            );
        }

        diffuseTexture->Unbind();
        specularTexture->Unbind();
    }
}

void Renderer::OnEvent(Event& e)
{
    ;
    if (e.GetType() == WindowResizeEvent::GetStaticType())
    {
        auto& resize = static_cast<WindowResizeEvent&>(e);
        glViewport(0, 0, resize.Width, resize.Height);
    }
}
