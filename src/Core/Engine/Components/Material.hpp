#pragma once

#include <memory>
#include <optional>
#include <glm/glm.hpp>

#include "Engine/Renderer/API/Texture2D.hpp"
#include "Engine/Renderer/API/Shader.hpp"

using MaterialID = uint64_t;

struct alignas(16) MaterialGPU
{
    glm::vec3 ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
    float shininess = 16.0f;

    float transparency = 1.0f;
    float _pad0 = 0.f;
    float _pad1 = 0.f;
    float _pad2 = 0.f;
};

struct Material
{
    ShaderID shader = static_cast<ShaderID>(-1);
    TextureID diffuse = static_cast<TextureID>(-1);
    TextureID specular = static_cast<TextureID>(-1);
    MaterialGPU data;
};

struct MaterialOverride
{
    std::optional<float> transparency;
    std::optional<float> shininess;
};

struct COMPMaterial
{
    MaterialID material;
};