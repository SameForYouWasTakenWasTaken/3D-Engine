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
    MaterialID Load(ShaderID shader, TextureID diffuse);
    MaterialID Load(const Material& material);
    Material* Get(MaterialID id);
};