#pragma once

#include <vector>
#include <glad/glad.h>


class EBO final {
    GLuint id;
public:
    EBO();
    ~EBO();

    void SetData(std::vector<GLuint>& indices, GLenum draw_type = GL_STATIC_DRAW);
    void Bind();
    void Unbind();
};