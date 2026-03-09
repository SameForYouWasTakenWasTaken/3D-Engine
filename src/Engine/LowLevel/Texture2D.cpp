#include <Engine/LowLevel/Texture2D.hpp>

void UploadTexture(unsigned char* data, int width, int height, int nrChannels)
{
    GLenum format, internal;
    if (nrChannels == 1) { format = internal = GL_RED; }
    else if (nrChannels == 2) { format = GL_RG; internal = GL_RG8; }
    else if (nrChannels == 3) { format = GL_RGB; internal = GL_RGB8; }
    else if (nrChannels == 4) { format = GL_RGBA; internal = GL_RGBA8; }

    GLint alignment = 1;
    if ((width * nrChannels) % 8 == 0) alignment = 8;
    else if ((width * nrChannels) % 4 == 0) alignment = 4;
    else if ((width * nrChannels) % 2 == 0) alignment = 2;

    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}


Texture2D::Texture2D(const std::string& filepath, TextureSettings s)
: texture_filepath(filepath), settings(s)
{
    glGenTextures(1, &id);
    Recreate(filepath); // Functionality already present
}

Texture2D::Texture2D()
{}

Texture2D::~Texture2D()
{

}

void Texture2D::Bind()
{
    glBindTexture(GL_TEXTURE_2D, id); // glIsTexture only works if a texture is bound. Henceforth it is in the beginning.
    if (!glIsTexture(id))
    {
        logger.AppendLogTag("TEXTURE_2D", LogColors::CYAN);
        logger.LogError("Texture not bound!");
        logger.DumpLogs();
        loaded = false;
        return;
    }
}

void Texture2D::Unbind(){
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Recreate(const std::string& filepath)
{
    texture_filepath = filepath; // Set new path then continue recreation proccess
    Recreate();
}

void Texture2D::Recreate()
{
    Bind();
    if (!glIsTexture(id))
    {
        if (texture_filepath.empty())
        {
            logger.AppendLogTag("TEXTURE_2D", LogColors::CYAN);
            logger.LogError("Provide a path for your texture!");
            logger.DumpLogs();
            return;
        }
    }
    
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.Texture_Style_X);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.Texture_Style_Y);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.Texture_Res_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.Texture_Res_Mag);

    //                            ->            Private member variables           <-
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texture_filepath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
       Bind();
       UploadTexture(data, width, height, nrChannels);
       if (glIsTexture(id) == GL_TRUE) loaded = true;
    }
    else {
        logger.AppendLogTag("TEXTURE_2D", LogColors::CYAN);
        logger.LogError("Failed to load texture!");
        logger.DumpLogs();
    }

    stbi_image_free(data);
}

void Texture2D::SetSettings(TextureSettings s)
{
    settings = s;
}

void Texture2D::Use(GLenum type)
{
    glActiveTexture(type);
    Bind();
}