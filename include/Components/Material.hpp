#pragma once

#include <memory>
#include <entt.hpp>
#include <glm/glm.hpp>
#include <Engine/LowLevel/Shader.hpp>

using TextureID = uint32_t;
using ShaderID = uint32_t;
using MaterialID = uint32_t;

struct Material
{
    ShaderID shader;
    TextureID texture;
};

struct COMPMaterial
{
    MaterialID material;
};