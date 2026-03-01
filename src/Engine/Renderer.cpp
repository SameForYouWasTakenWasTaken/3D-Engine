#include <Engine/Renderer.hpp>

Renderer::Renderer(EngineContext& context) 
    : m_EngineContext(context), m_SceneManager(std::make_unique<SceneManager>(context)) 
{ };

// Deprecated
void Renderer::Draw(entt::registry& registry)
{
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 projection = glm::mat4(1.f);
    float near = 0.01;
    float far = 1000;

    float fov = 90.f;
    float aspect_ratio = 800.f / 600.f;

    projection = glm::perspective(glm::radians(fov), aspect_ratio, near, far);
    view = glm::translate(view, {0.f, 0.f, -2.f});

    AttributePointer pos;
    pos.index = 0;
    pos.size = 3;
    pos.offset = (void*)offsetof(Vertex, position);
    
    AttributePointer col;
    col.index = 1;
    col.size = 4;
    col.offset = (void*)offsetof(Vertex, color);
    
    AttributePointer tex;
    tex.index = 2;
    tex.size = 2;
    tex.offset = (void*)offsetof(Vertex, tex_coords);

    // auto Drawables = registry.view<COMPGeometry, COMPTransform>();
    // Drawables.each([&](auto entity, COMPGeometry& drawable, COMPTransform& transform){
    //     VAO vao;
    //     VBO vbo;
    //     EBO ebo;

    //     vao.Bind();
        
    //     vbo.Bind();
    //     vbo.SetData(drawable.vertices);
        
    //     ebo.Bind();
    //     ebo.SetData(drawable.indices);

    //     vao.LinkAttrib(pos);
    //     vao.LinkAttrib(col);
    //     vao.LinkAttrib(tex);

    //     vao.Unbind();
    //     vbo.Unbind();
    //     ebo.Unbind();
    //     glm::mat4 model = glm::mat4(1.f);
    //     model = glm::translate(model, transform.position);
    //     model = glm::rotate(model, glm::radians(transform.rotation.x), {1,0,0});
    //     model = glm::rotate(model, glm::radians(transform.rotation.y), {0,1,0});
    //     model = glm::rotate(model, glm::radians(transform.rotation.z), {0,0,1});
    //     model = glm::scale(model, transform.scale);

    //     drawable.shader->UseProgram();
    //     drawable.shader->SetMatrix4("model", 1, glm::value_ptr(model));
    //     drawable.shader->SetMatrix4("PVmat", 1, glm::value_ptr(projection * view));
    //     vao.Bind();
    //     glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(drawable.indices.size()), GL_UNSIGNED_INT, nullptr);
    // });
    
}

// TODO: Batch updating
void Renderer::Update(float dt)
{
    if (m_SceneManager->m_ActiveScene)
        m_SceneManager->m_ActiveScene->Update(dt);
}

void Renderer::Submit(Mesh* mesh, COMPMaterial* material, const COMPTransform& transform)
{
    // This is a hash that combines the mesh and material pointers to create a unique key for batching
    // It's a much faster way to batch than comparing the mesh and material pointers directly, 
    // and it allows us to easily store batches in an unordered_map
    size_t batchKey =
    std::hash<Mesh*>{}(mesh) ^
    (std::hash<COMPMaterial*>{}(material) << 1);

    InstanceData instanceData {transform.model};
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

void Renderer::End()
{
    if (m_SceneManager->m_ActiveScene == nullptr)
    {
        logger.AppendLogTag("RENDERER", LogColors::RED);
        logger.LogWarning("No active scene set, skipping render.");
        logger.DumpLogs();
        return;
    }

    m_SceneManager->m_ActiveScene->Draw(*this);

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
        glDrawElementsInstanced(
            GL_TRIANGLES, 
            batch.mesh->IndexCount, 
            GL_UNSIGNED_INT, 
            nullptr, 
            static_cast<GLsizei>(batch.instances.size())
        );
    }
}

void Renderer::OnEvent(Event& e)
{
    auto activeScene = m_SceneManager->m_ActiveScene;
    if (activeScene)
        activeScene->OnEvent(e);
    
    if (auto *resizeEvent = dynamic_cast<WindowResizeEvent*>(&e))
    {
        glViewport(0, 0, resizeEvent->Width, resizeEvent->Height);
    }
}