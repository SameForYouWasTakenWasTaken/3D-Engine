#include "UBO.hpp"

UBO::UBO()
{
    glGenBuffers(1, &id);
}

UBO::~UBO()
{
    glDeleteBuffers(1, &id);
}

void UBO::SetData(GLsizeiptr size, const void* data, GLenum usage)
{
    glBufferData(
        GL_UNIFORM_BUFFER,
        size,
        data,
        usage
    );
}

void UBO::SetBinding(GLuint n) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, n, id);
}

void UBO::Bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
}

void UBO::Unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
