#pragma once

#include <vendor/glad.h>


class FBO final
{
    GLuint m_ColorTexture;
    GLuint m_RBO;

    float m_Width, m_Height = 1.f;
public:
    GLuint id;
    FBO();
    ~FBO();

    FBO(const FBO&) = delete;
    FBO& operator=(const FBO&) = delete;

    FBO(FBO&& other) noexcept : id(other.id) {
        other.id = 0;
    }

    void Bind();

    void Create(int width, int height);
    void Resize(float width, float height);

    /**
 * @brief Retrieves the OpenGL texture handle used as the framebuffer's color attachment.
 *
 * @return GLuint OpenGL texture handle for the color attachment.
 */
GLuint GetColorTexture() const { return m_ColorTexture; }
    /**
 * @brief Retrieves the OpenGL renderbuffer object attached to this framebuffer.
 *
 * @return GLuint OpenGL renderbuffer object (RBO) ID associated with the FBO.
 */
GLuint GetRBO() const { return m_RBO; }

    float GetWidth();
    float GetHeight();

    static void Unbind();
    static bool CheckStatus();
};