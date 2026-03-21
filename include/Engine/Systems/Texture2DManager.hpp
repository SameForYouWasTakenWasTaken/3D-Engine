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
    Texture2DManager() = default;
    Texture2DManager(const Texture2DManager&) = delete;

    [[nodiscard]]
    std::optional<TextureID> Load(const std::string& path);

    [[nodiscard]]
    std::optional<TextureID> Load(std::shared_ptr<Texture2D> texture);

    [[nodiscard]]
    std::shared_ptr<Texture2D> Get(TextureID id);
};
