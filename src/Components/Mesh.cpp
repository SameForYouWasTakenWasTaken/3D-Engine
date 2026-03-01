#include <Components/Mesh.hpp>

void Mesh::SetData(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLenum draw_type)
{

    vao.Bind();
    
    vbo.Bind();
    vbo.SetData(vertices, draw_type);
    
    ebo.Bind();
    ebo.SetData(indices, draw_type);

    vao.LinkAttrib(GetAttribPointerPos());
    vao.LinkAttrib(GetAttribPointerCol());
    vao.LinkAttrib(GetAttribPointerTex());

    

    instanceVBO.Bind();

    // Allocate empty buffer for instance transforms
    constexpr size_t MAX_INSTANCES = 10000;

    glBufferData(
        GL_ARRAY_BUFFER,
        MAX_INSTANCES * sizeof(glm::mat4),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    // Configure matrix attributes (11–14)
    std::size_t vec4Size = sizeof(glm::vec4);

    for (int i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(11 + i);

        glVertexAttribPointer(
            11 + i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::mat4),
            (void*)(i * vec4Size)
        );

        // Important for instancing
        glVertexAttribDivisor(11 + i, 1);
    }

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

    IndexCount = indices.size();
}