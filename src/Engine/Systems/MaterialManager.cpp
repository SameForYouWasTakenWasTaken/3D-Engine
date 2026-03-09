#include <Engine/Systems/MaterialManager.hpp>

MaterialID MaterialManager::CreateMaterial(
    ShaderID shader,  
    TextureID texture)
{
    MaterialID id = m_NextMaterialID++;

    m_Materials.emplace(id, Material{
        shader,
        texture
    });

    return id;
}

Material* MaterialManager::Get(MaterialID id) 
{ 
    auto it = m_Materials.find(id);
    if (it == m_Materials.end())
        return nullptr;

    return &it->second;
}
