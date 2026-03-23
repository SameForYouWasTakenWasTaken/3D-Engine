#include <iostream>
#include <Components/Mesh.hpp>

/**
 * @brief Initialize mesh geometry and configure the VAO for rendering, including instanced transforms.
 *
 * Uploads vertex and index data to the GPU, allocates a buffer for up to 10,000 per-instance transformation matrices,
 * enables and configures vertex attributes for position, color, texture coordinates, and the four matrix row attributes
 * (locations 11–14) with an attribute divisor of 1 to support instanced rendering. Updates the mesh's IndexCount and
 * VertexCount to reflect the provided data.
 *
 * @param vertices Vector of vertex data to upload to the vertex buffer.
 * @param indices  Vector of element indices to upload to the index buffer.
 * @param draw_type OpenGL buffer usage hint used when uploading vertex and index data (e.g., `GL_STATIC_DRAW`, `GL_DYNAMIC_DRAW`).
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