#include <Engine/Systems/MaterialManager.hpp>

MaterialID MaterialManager::Load(Material material)
{
    MaterialID id = Hash<MaterialID>(
        material.shader,
        material.diffuse,
        material.specular,
        material.ambient.x,
        material.ambient.y,
        material.ambient.z,
        material.shininess,
        material.transparency
        );
    if (Get(id) != nullptr)
        return id;
    m_Materials.emplace(id, std::move(material));
    return id;
}

std::optional<MaterialID> MaterialManager::Clone(MaterialID id)
{
    auto it = m_Materials.find(id);
    if (it == m_Materials.end())
        return std::nullopt;


    auto newID = m_NextMaterialID++;
    m_Materials.emplace(newID, it->second);
    return newID;
}

Material* MaterialManager::Get(MaterialID id) 
{ 
    auto it = m_Materials.find(id);
    if (it == m_Materials.end())
        return nullptr;
    return &it->second;
}
