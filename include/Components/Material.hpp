#pragma once

#include <memory>
#include <optional>
#include <glm/glm.hpp>

#include <Engine/LowLevel/Texture2D.hpp>
#include <Engine/LowLevel/Shader.hpp>

using MaterialID = uint64_t;

struct Material
{
    ShaderID shader = static_cast<ShaderID>(-1);
    TextureID diffuse = static_cast<TextureID>(-1);
    TextureID specular = static_cast<TextureID>(-1);

    glm::vec3 ambient  = glm::vec3(0.1f, 0.1f, 0.1f);  // low ambient contribution

    float transparency = 1.0f;
    float shininess = 32.0f;
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