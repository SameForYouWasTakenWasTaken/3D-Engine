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
    MaterialManager(const MaterialManager&) = delete;
    [[nodiscard]]
    MaterialID Load(Material material);

    [[nodiscard]]
    Material* Get(MaterialID id);
};