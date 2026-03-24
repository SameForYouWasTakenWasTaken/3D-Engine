#pragma once
#include <unordered_map>
#include <memory>
#include <optional>

#include <App/Services.hpp>
#include <Engine/LowLevel/Texture2D.hpp>
#include <Components/Texture.hpp>


class Texture2DManager : public IService
{
    std::unordered_map<TextureID, std::shared_ptr<Texture2D>> m_Textures;
public:
    /**
 * @brief Constructs a Texture2DManager with an empty texture registry.
 *
 * Initializes the manager in its default state; no textures are loaded or registered.
 */
Texture2DManager() = default;
    /**
 * @brief Disables copy construction.
 *
 * Prevents creating a new Texture2DManager by copying an existing instance.
 */
Texture2DManager(const Texture2DManager&) = delete;

    [[nodiscard]]
    std::optional<TextureID> Load(const std::string& path);

    [[nodiscard]]
    std::optional<TextureID> Load(std::shared_ptr<Texture2D> texture);

    [[nodiscard]]
    std::shared_ptr<Texture2D> Get(TextureID id);
};
