#include <Engine/LowLevel/EBO.hpp>

EBO::EBO()
{
    glGenBuffers(1, &id);
}
EBO::~EBO()
{
    glDeleteBuffers(1, &id);
}

void EBO::SetData(std::vector<GLuint>& indices, GLenum usage)
{
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), usage);
}

void EBO::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}