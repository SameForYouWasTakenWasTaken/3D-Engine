#pragma once

#include <glad/glad.h>


class FBO final
{
    GLuint id;
    GLuint m_ColorTexture;
    GLuint m_RBO;
public:
    FBO();
    ~FBO();

    void Bind();

    void Create(int width, int height);
    void Resize(int width, int height);

    GLuint GetColorTexture() const { return m_ColorTexture; }
    GLuint GetRBO() const { return m_RBO; }

    static void Unbind();
    static bool CheckStatus();
};