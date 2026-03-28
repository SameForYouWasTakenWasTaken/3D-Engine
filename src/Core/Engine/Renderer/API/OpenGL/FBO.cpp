#include <iostream>
#include "FBO.hpp"

#include "tracy/TracyOpenGL.hpp"

/**
 * @brief Initializes the FBO instance and creates an OpenGL framebuffer object.
 *
 * Initializes internal attachment handles to zero and generates an OpenGL
 * framebuffer name stored in `id`.
 */
FBO::FBO() : m_ColorTexture(0), m_RBO(0)
{
    glGenFramebuffers(1, &id);
}

/**
 * @brief Releases the OpenGL framebuffer object owned by this instance.
 *
 * Deletes the framebuffer whose handle is stored in `id`.
 *
 * @note This destructor does not delete the color texture or renderbuffer attachments; those are released when resized or managed elsewhere.
 */
FBO::~FBO()
{
    if (m_ColorTexture != 0)
    {
        glDeleteTextures(1, &m_ColorTexture);
        m_ColorTexture = 0;
    }

    if (m_RBO != 0)
    {
        glDeleteRenderbuffers(1, &m_RBO);
        m_RBO = 0;
    }

    if (id != 0)
    {
        glDeleteFramebuffers(1, &id);
        id = 0;
    }
}

/**
 * @brief Creates and attaches color and depth-stencil buffers for this FBO at the specified size.
 *
 * Allocates a 2D RGB color texture and a depth-stencil renderbuffer sized to the provided width and height,
 * attaches them to this framebuffer, verifies framebuffer completeness, and leaves the FBO unbound.
 * If the framebuffer is not complete, a diagnostic message is written to standard output.
 *
 * @param width Width of the attachments in pixels.
 * @param height Height of the attachments in pixels.
 */
void FBO::Create(int width, int height)
{
    // 1. Create color texture
    Bind();
    glDeleteTextures(1, &m_ColorTexture);
    glGenTextures(1, &m_ColorTexture);
    glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    // Texture setup
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        m_ColorTexture,
        0
    );

    // 2. Create renderbuffer for depth + stencil
    glDeleteRenderbuffers(1, &m_RBO);
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        m_RBO
    );

    // 3. Check completeness
    if (!CheckStatus())
    {
        std::cout << "FBO is incomplete!\n";
    }

    Unbind();

    m_Width = width;
    m_Height = height;
}

/**
 * @brief Rebuilds the framebuffer attachments to the specified dimensions.
 *
 * Deletes the existing color texture and depth-stencil renderbuffer (if present)
 * and recreates attachments at the provided width and height.
 *
 * @param width New framebuffer width in pixels.
 * @param height New framebuffer height in pixels.
 */
void FBO::Resize(double width, double height)
{
    if (width <= 0 || height <= 0)
        return;
    // delete old
    if (m_ColorTexture)
        glDeleteTextures(1, &m_ColorTexture);

    if (m_RBO)
        glDeleteRenderbuffers(1, &m_RBO);

    // recreate with new size
    Create(width, height);
}


/**
 * @brief Makes this FBO the current framebuffer bound to GL_FRAMEBUFFER.
 *
 * Subsequent framebuffer operations will target this FBO until another framebuffer is bound.
 */
void FBO::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

float FBO::GetWidth()
{
    return m_Width;
}

float FBO::GetHeight()
{
    return m_Height;
}

/**
 * @brief Restores rendering to the default framebuffer by binding framebuffer 0.
 */
void FBO::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Checks whether the currently bound framebuffer is complete.
 *
 * Queries OpenGL for the status of GL_FRAMEBUFFER and reports completeness.
 *
 * @return true if the currently bound framebuffer is complete, false otherwise.
 */
bool FBO::CheckStatus()
{
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        return true;
    return false;
}
