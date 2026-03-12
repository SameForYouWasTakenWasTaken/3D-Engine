#pragma once
#include <glad/glad.h>
#include <Logger.hpp>
#include <stb_image.h>

// Standard settings you might modify
struct TextureSettings {
    // Use XY instead of ST to prevent confusion
    GLenum Texture_Style_X = GL_MIRRORED_REPEAT;
    GLenum Texture_Style_Y = GL_MIRRORED_REPEAT;

    // Resolution stuff
    GLenum Texture_Res_Min = GL_LINEAR_MIPMAP_LINEAR;
    GLenum Texture_Res_Mag = GL_LINEAR;
};

class Texture2D final {
    GLuint id;    
    TextureSettings settings;
    std::string texture_filepath;
    int width, height, nrChannels;
    bool loaded = false;
public:
    Logger logger = Logger("TEXTURE_2D");
    Texture2D(const std::string& filepath, TextureSettings s = TextureSettings()); // Default texture settings already provided
    Texture2D();
    ~Texture2D();

    
    void SetSettings(TextureSettings s);
    void Recreate(const std::string& filepath);
    void Recreate();
    void Bind();
    void Unbind();

    /*
    You can draw by binding() and running the necessary OpenGL draw functions, but it's recommended to use
    Use() before drawing, as it can help prevent bugs on other machines. Bind() is typically used for
    changing the texture parameters if the TextureSettings struct wasn't enough.
    */
    void Use(GLenum type = GL_TEXTURE0);

    GLuint GetTexture() const {return id;}
    bool IsLoaded() const {return loaded;}
};