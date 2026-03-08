#pragma once

#include <Engine/Systems/ShaderManager.hpp>
#include <Components/Material.hpp>

class MaterialManager
{
private:
    std::unordered_map<MaterialID, Material> m_Materials;
    uint32_t m_NextMaterialID = 0;
    ShaderManager& m_ShaderManager;

public:
    MaterialManager(ShaderManager& shaderManager) : m_ShaderManager(shaderManager) {}
    MaterialID CreateMaterial(ShaderID shader, TextureID texture);
    Material* Get(MaterialID id);
};