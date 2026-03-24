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

/**
 * @brief Unbinds any vertex buffer from the GL_ARRAY_BUFFER target.
 *
 * Ensures no VBO is bound to GL_ARRAY_BUFFER by binding zero to that target.
 */
void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}