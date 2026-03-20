#pragma once

#include <memory>
#include <glm/glm.hpp>

using TextureID = uint32_t;
using ShaderID = uint32_t;
using MaterialID = uint32_t;

struct Material
{
    ShaderID shader = static_cast<TextureID>(-1);
    TextureID diffuse = static_cast<TextureID>(-1);
    TextureID specular = static_cast<TextureID>(-1);

    glm::vec3 ambient  = glm::vec3(0.1f, 0.1f, 0.1f);  // low ambient contribution

    float transparency = 1.0f;
    float shininess = 32.0f;
};

struct COMPMaterial
{
    MaterialID material;
};