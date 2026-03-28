#pragma once

#include <vendor/glad.h>

class UBO final
{
public:
    GLuint id;
    UBO();
    ~UBO();
    UBO(const UBO&) = delete;
    UBO& operator=(const UBO&) = delete;
    UBO(UBO&& other) noexcept : id(other.id) {
        other.id = 0;
    }

    void SetData(GLsizeiptr size, const void* data, GLenum usage = GL_STREAM_DRAW);
    void SetBinding(GLuint n) const;
    void Bind();
    static void Unbind();
};