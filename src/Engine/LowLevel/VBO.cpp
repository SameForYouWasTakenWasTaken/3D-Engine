#include <Engine/LowLevel/VBO.hpp>

VBO::VBO()
{
    glGenBuffers(1, &id);
}

VBO::~VBO()
{
    glDeleteBuffers(1, &id);
}

void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}