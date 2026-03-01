#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <Engine/LowLevel/Shader.hpp>

struct COMPMaterial
{
    std::unique_ptr<Shader> shader;
    glm::vec3 color;
    
    // float roughness;
    // Texture2D* albedo;
    // Texture2D* normal;
};