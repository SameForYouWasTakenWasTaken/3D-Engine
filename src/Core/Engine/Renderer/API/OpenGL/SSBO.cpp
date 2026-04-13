#include "SSBO.hpp"

SSBO::SSBO()
{
    glGenBuffers(1, &id);
}

SSBO::~SSBO()
{
    glDeleteBuffers(1, &id);
}

void SSBO::SetBufferData(GLsizeiptr size, const void* data, GLenum usage)
{
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        size,
        data,
        usage
    );
}

void SSBO::Bind()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
}

void SSBO::SetBinding(int n)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, n, id);
}

void SSBO::Unbind()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}