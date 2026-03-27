#pragma once

#include <vector>
#include "vendor/glad.h"


class EBO final {
public:
    GLuint id;
    EBO();
    ~EBO();
    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;

    EBO(EBO&& other) noexcept : id(other.id) {
        other.id = 0;
    }

    void SetData(std::vector<GLuint>& indices, GLenum draw_type = GL_STATIC_DRAW);
    void Bind();
    void Unbind();
};