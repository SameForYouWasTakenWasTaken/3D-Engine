#include <iostream>
#include <Components/Mesh.hpp>

/**
 * @brief Uploads mesh vertex and index data to the GPU and configures the VAO for rendering and instancing.
 *
 * Allocates a dynamic per-instance buffer for up to 10,000 InstanceData entries and configures per-vertex attributes
 * (position, color, texture coordinates, normal) plus instanced attributes for the model and normal matrices with
 * attribute divisors set for per-instance advancement. Updates IndexCount, VertexCount, and Indexed to reflect the
 * mesh's current data.
 *
 * @param draw_type OpenGL usage hint used when uploading vertex and index buffers (e.g., `GL_STATIC_DRAW`, `GL_DYNAMIC_DRAW`).
 */
void Mesh::SetData(GLenum draw_type)
{
    vao.Bind();

    // Vertex buffer
    vbo.Bind();
    vbo.SetData(vertices, draw_type);

    // Element buffer
    ebo.Bind();
    ebo.SetData(indices, draw_type);

    // Per-vertex attributes
    vao.LinkAttrib(GetAttribPointerPos());  // location 0
    vao.LinkAttrib(GetAttribPointerCol());  // location 1
    vao.LinkAttrib(GetAttribPointerTex());  // location 2
    vao.LinkAttrib(GetAttribPointeraNormal()); // location 3

    // Bind instance VBO first
    instanceVBO.Bind();
    constexpr size_t MAX_INSTANCES = 10000;
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);

    size_t vec4Size = sizeof(glm::vec4);

    // Model matrix: locations 4–7
    for (int i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(i * vec4Size));
        glVertexAttribDivisor(4 + i, 1);
    }

    // Normal matrix: locations 8–10
    for (int i = 0; i < 3; ++i)
    {
        glEnableVertexAttribArray(8 + i);
        glVertexAttribPointer(8 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(64 + i * vec4Size));
        glVertexAttribDivisor(8 + i, 1);
    }

    // Cleanup
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
    instanceVBO.Unbind();
    IndexCount = indices.size();
    VertexCount = vertices.size();
    Indexed = !indices.empty();
}