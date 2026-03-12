#pragma once

#include <memory>
#include <glm/glm.hpp>

using TextureID = uint32_t;
using ShaderID = uint32_t;
using MaterialID = uint32_t;

struct Material
{
    ShaderID shader = -1;
    TextureID diffuse = -1;
    TextureID specular = -1;

    glm::vec3 ambient  = glm::vec3(0.1f, 0.1f, 0.1f);  // low ambient contribution

    float shininess = 32.0f;
};

struct COMPMaterial
{
    MaterialID material;
};