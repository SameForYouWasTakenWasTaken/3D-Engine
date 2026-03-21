#include <Engine/Systems/Texture2DManager.hpp>

#include "glm/gtc/type_ptr.hpp"

std::optional<TextureID> Texture2DManager::Load(const std::string& path)
{
    auto hash = Hash<TextureID>(path);
    if (Get(hash) != nullptr) return hash;

    auto texture = std::make_shared<Texture2D>(path);
    if (texture->IsLoaded())
    {
        m_Textures.emplace(hash, texture);
        return hash;
    }

    return std::nullopt;
}

std::optional<TextureID> Texture2DManager::Load(const std::shared_ptr<Texture2D> texture)
{
    auto hash = Hash<TextureID>(texture->GetPath());
    if (Get(hash) != nullptr) return hash;

    if (texture->IsLoaded())
    {
        m_Textures.emplace(hash, texture);
        return hash;
    }

    return std::nullopt;
}

std::shared_ptr<Texture2D> Texture2DManager::Get(TextureID id)
{
    auto it = m_Textures.find(id);
    if (it == m_Textures.end()) return nullptr;
    return it->second;
}