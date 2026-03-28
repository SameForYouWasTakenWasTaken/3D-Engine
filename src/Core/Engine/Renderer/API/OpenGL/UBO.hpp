#pragma once

#include "vendor/glad.h"

class UBO final
{
public:
    GLuint id;
    UBO();

    void SetData(GLsizeiptr size, const void* data, GLenum usage = GL_STREAM_DRAW);
    void SetBinding(GLuint n) const;
    void Bind();
    static void Unbind();
};