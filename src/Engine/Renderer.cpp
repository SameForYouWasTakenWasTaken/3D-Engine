#include <Engine/Renderer.hpp>

Renderer::Renderer(EngineContext& context) 
    : m_EngineContext(context), m_SceneManager(std::make_unique<SceneManager>(context)) 
{ };

// TODO: Batch updating
void Renderer::Update(float dt)
{

}

void Renderer::Submit(Mesh* mesh, COMPMaterial* material, const glm::mat4& model)
{
    // This is a hash that combines the mesh and material pointers to create a unique key for batching
    // It's a much faster way to batch than comparing the mesh and material pointers directly, 
    // and it allows us to easily store batches in an unordered_map
    size_t batchKey =
    std::hash<Mesh*>{}(mesh) ^
    (std::hash<COMPMaterial*>{}(material) << 1);

    InstanceData instanceData {model};
    auto& batch = m_Batches[batchKey];
    
    // Assign the necessary stuff
    batch.mesh = mesh;
    batch.material = material;

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
        Mesh* mesh = batch.mesh;
        COMPMaterial* material = batch.material;

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

        material->shader->UseProgram();

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