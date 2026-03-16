#pragma once

#include <App/Services.hpp>
#include <Engine/Systems/ShaderManager.hpp>
#include <Components/Material.hpp>

class MaterialManager : public IService
{
    std::unordered_map<MaterialID, Material> m_Materials;
    uint32_t m_NextMaterialID = 0;
public:
    MaterialManager() = default;
    MaterialID CreateMaterial(ShaderID shader, TextureID texture);
    Material* Get(MaterialID id);
};