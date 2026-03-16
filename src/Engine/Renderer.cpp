#include <Engine/Renderer.hpp>

Renderer::Renderer(EngineContext& context) 
    : m_EngineContext(context) 
{ };

// TODO: Batch updating
void Renderer::Update(float dt)
{

}

void Renderer::Submit(Mesh* mesh, uint32_t materialID, const glm::mat4& model, std::shared_ptr<LightManager> lightManager)
{
    // This is a hash that combines the mesh and material pointers to create a unique key for batching
    // It's a much faster way to batch than comparing the mesh and material pointers directly, 
    // and it allows us to easily store batches in an unordered_map
    size_t batchKey =
        std::hash<Mesh*>{}(mesh) ^
        (std::hash<uint32_t>{}(materialID) << 1) ^
        (std::hash<LightManager*>{}(lightManager.get()) << 2);


    InstanceData instanceData{};
    instanceData.model = model;
    // prevent unnecessary initialization
    auto [it, inserted] = m_Batches.try_emplace(batchKey);
    auto& batch = it->second;
    
    // Assign the necessary stuff
    batch.mesh = mesh;
    batch.materialID = materialID;
    batch.lightManager = lightManager;

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

    // Loop through each batch and render it
    for (auto& [batchKey, batch] : m_Batches)
    {
        if (batch.instances.empty())
            continue;

        auto& services = Services::Get();
        auto& materialManager = services.GetService<MaterialManager>();
        auto& textureManager = services.GetService<Texture2DManager>();
        auto& shaderManager = services.GetService<ShaderManager>();

        Mesh* mesh = batch.mesh;

        Material* material = materialManager.Get(batch.materialID );

        Shader* shader = shaderManager.Get(material->shader);

        Texture2D* diffuseTexture = textureManager.Get(material->diffuse);
        Texture2D* specularTexture = textureManager.Get(material->specular);

        std::shared_ptr<LightManager> lightManager = batch.lightManager;

        glm::vec3 camPos = m_EngineContext.cached_activeCam_position;

        if(!material || !shader || !mesh) continue;

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

        //Calculate normals on the CPU
        for (auto& instData : batch.instances)
        {
            glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(instData.model)));

            instData.normal0 = glm::vec4(normalMat[0], 0.0f);
            instData.normal1 = glm::vec4(normalMat[1], 0.0f);
            instData.normal2 = glm::vec4(normalMat[2], 0.0f);
        }

        // TODO: Make sure you use default textures if none provided
        diffuseTexture->Use();
        mesh->vao.Bind();
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
        shader->SetMatrix4(
            "projectmat", 
            1, 
            glm::value_ptr(m_EngineContext.cached_projection)
        );
        shader->SetMatrix4(
            "viewmat", 
            1, 
            glm::value_ptr(m_EngineContext.cached_view)
        );
        shader->SetVec3(
            "viewPos", 
            camPos
        );

        shader->SetVec3("material.ambient", material->ambient);
        shader->SetInt("material.diffuse", 0); // Texture slot 0, activated on Texture->Use() above
        shader->SetInt("material.specular", 1); // Slot 1
        shader->SetFloat("material.shininess", material->shininess);
        lightManager->UploadToShader(shader);
        
        if (mesh->Indexed)
        {
            glDrawElementsInstanced(
                mesh->Primitive, 
                batch.mesh->IndexCount, 
                GL_UNSIGNED_INT, 
                nullptr, 
                static_cast<GLsizei>(batch.instances.size())
            );
        }else{
            glDrawArraysInstanced(
                mesh->Primitive,
                0,
                mesh->VertexCount,
                instanceCount
            );
        }

        diffuseTexture->Unbind();
    }
}

void Renderer::OnEvent(Event& e)
{;
    if (e.GetType() == WindowResizeEvent::GetStaticType())
    {
        auto& resize = static_cast<WindowResizeEvent&>(e);
        glViewport(0, 0,resize.Width,resize.Height);
    }
}