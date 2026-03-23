#include <iostream>
#include <Engine/LowLevel/FBO.hpp>

FBO::FBO() : m_ColorTexture(0), m_RBO(0)
{
    glGenFramebuffers(1, &id);
}

FBO::~FBO()
{
    glDeleteFramebuffers(1, &id);
}

void FBO::Create(int width, int height)
{
    // 1. Create color texture
    Bind();
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
}

void FBO::Resize(int width, int height)
{
    // delete old
    if (m_ColorTexture)
        glDeleteTextures(1, &m_ColorTexture);

    if (m_RBO)
        glDeleteRenderbuffers(1, &m_RBO);

    // recreate with new size
    Create(width, height);
}


void FBO::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FBO::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool FBO::CheckStatus()
{
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        return true;
    return false;
}
