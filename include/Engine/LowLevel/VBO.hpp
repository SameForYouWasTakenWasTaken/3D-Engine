#pragma once
#include <vector>

#include <glad/glad.h>

#include <Engine/LowLevel/Vertex.hpp>

class VBO final {
    GLuint id;
public:
    VBO();
    ~VBO();

    template<typename T>
    void SetData(std::vector<T>& vertices, GLenum draw_type = GL_STATIC_DRAW);

    void Bind();
    static void Unbind();

};

template <typename T>
void VBO::SetData(std::vector<T>& vertices, GLenum draw_type)
{
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), draw_type);
}
