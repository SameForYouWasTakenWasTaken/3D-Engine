#include <Engine/Systems/Texture2DManager.hpp>

#include "glm/gtc/type_ptr.hpp"

/**
 * @brief Loads a texture from a file path and returns its computed texture ID.
 *
 * If a texture for the given path is already cached, the existing texture ID is returned.
 * If the texture is not cached, attempts to construct and load a Texture2D from the path;
 * on successful load the texture is inserted into the manager's cache and its ID is returned.
 *
 * @param path Filesystem path to the texture asset.
 * @return std::optional<TextureID> The computed texture ID if the texture is cached or successfully loaded, `std::nullopt` if loading failed.
 */
std::optional<TextureID> Texture2DManager::Load(const std::string& path, TextureSettings settings)
{
    auto hash = Hash<TextureID>(path);
    if (Get(hash) != nullptr) return hash;

    auto [it, inserted] = m_Textures.try_emplace(hash, Texture2D(path, settings));
    if (!inserted)
        return hash;

    if (!it->second.IsLoaded())
    {
        m_Textures.erase(it);
        return std::nullopt;
    }

    return hash;

    return std::nullopt;
}

/**
 * Loads and caches a provided Texture2D and returns its TextureID.
 *
 * If a texture with the same path is already cached, returns that existing ID.
 *
 * @param texture Shared pointer to the Texture2D to cache; the texture's path is used to derive the ID.
 * @return std::optional<TextureID> `TextureID` for the cached (or newly inserted) texture, `std::nullopt` if the provided texture is not loaded.
 */
std::optional<TextureID> Texture2DManager::Load(Texture2D&& texture)
{
    if (!texture.IsLoaded())
        return std::nullopt;

    auto hash = Hash<TextureID>(texture.GetPath());
    auto [it, inserted] = m_Textures.try_emplace(hash, std::move(texture));
    return hash;
}

/**
 * Retrieve a managed Texture2D by its TextureID.
 *
 * @param id Identifier of the texture to look up.
 * @return std::shared_ptr<Texture2D> Pointer to the stored texture, or `nullptr` if no texture is associated with `id`.
 */
Texture2D* Texture2DManager::Get(TextureID id)
{
    auto it = m_Textures.find(id);
    if (it == m_Textures.end()) return nullptr;
    return &it->second;
}