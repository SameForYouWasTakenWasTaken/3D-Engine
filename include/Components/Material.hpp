#pragma once

#include <memory>
#include <entt.hpp>
#include <glm/glm.hpp>
#include <Engine/LowLevel/Shader.hpp>

using TextureID = size_t;
using ShaderID = uint32_t;
using MaterialID = uint32_t;

struct Material
{
    ShaderID shader;

    TextureID diffuse = -1;
    TextureID specular = -1;

    glm::vec3 ambient  = glm::vec3(0.1f, 0.1f, 0.1f);  // low ambient contribution

    float shininess = 32.0f;
};

struct COMPMaterial
{
    MaterialID material;
};