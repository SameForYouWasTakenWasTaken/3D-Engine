#include <Engine/Systems/Texture2DManager.hpp>

TextureID Texture2DManager::Hash(const std::string& path)
{
    return std::hash<std::string>{}(path);
}
TextureID Texture2DManager::Load(const std::string& path)
{
    TextureID id = Hash(path);
    if (m_Textures.find(id) != m_Textures.end()) return id;

    m_Textures[id] = std::make_shared<Texture2D>(path.c_str());
    return id;
}

Texture2D* Texture2DManager::Get(TextureID id)
{
    if (m_Textures.find(id) == m_Textures.end()) return nullptr;
    return m_Textures[id].get();
}