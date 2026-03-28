#pragma once
#include "vendor/glad.h"

class SSBO final
{
public:
    GLuint id;

    SSBO();
    ~SSBO();
    SSBO(const SSBO&) = delete;
    SSBO& operator=(const SSBO&) = delete;

    SSBO(SSBO&& other) noexcept : id(other.id) {
        other.id = 0;
    }

    void SetData(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW);
    void Bind();
    void SetBinding(int n);
    static void Unbind();
};
