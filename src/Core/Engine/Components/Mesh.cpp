#include <iostream>
#include "Mesh.hpp"

#include "tracy/Tracy.hpp"

/**
 * @brief Uploads mesh geometry and instance data to the GPU and configures the VAO for rendering with instancing.
 *
 * Uploads vertex and index data to their GPU buffers, allocates a dynamic per-instance buffer (capacity for 10,000
 * InstanceData entries), and configures per-vertex attributes (position, color, texture coordinates, normal) plus
 * per-instance attributes for the model matrix and normal matrix rows with attribute divisors set for per-instance
 * advancement. Also updates internal bookkeeping fields (`IndexCount`, `VertexCount`, `Indexed`) to reflect the mesh's
 * current data.
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
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);

    size_t vec4Size = sizeof(glm::vec4);

    // Model matrix: locations 4–7
    for (int i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(i * vec4Size));
        glVertexAttribDivisor(4 + i, 1);
    }

    // Normal rows
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
        reinterpret_cast<void*>(offsetof(InstanceData, normal0)));
    glVertexAttribDivisor(8, 1);

    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
        reinterpret_cast<void*>(offsetof(InstanceData, normal1)));
    glVertexAttribDivisor(9, 1);

    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
        reinterpret_cast<void*>(offsetof(InstanceData, normal2)));
    glVertexAttribDivisor(10, 1);

    // Cleanup
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
    instanceVBO.Unbind();
    IndexCount = indices.size();
    VertexCount = vertices.size();
    Indexed = !indices.empty();
}