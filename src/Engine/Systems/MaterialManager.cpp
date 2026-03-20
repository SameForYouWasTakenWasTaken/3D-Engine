#include <Engine/Systems/MaterialManager.hpp>

MaterialID MaterialManager::Load(
    ShaderID shader,  
    TextureID diffuse)
{
    MaterialID id = m_NextMaterialID++;

    m_Materials.emplace(id, Material{
        shader,
        diffuse
    });

    return id;
}

MaterialID MaterialManager::Load(const Material& material)
{
    MaterialID id = m_NextMaterialID++;
    m_Materials.emplace(id, material);
    return id;
}

Material* MaterialManager::Get(MaterialID id) 
{ 
    auto it = m_Materials.find(id);
    if (it == m_Materials.end())
        return nullptr;

    return &it->second;
}
