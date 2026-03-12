#pragma once
#include <unordered_map>
#include <memory>
#include <optional>

#include <Engine/LowLevel/Texture2D.hpp>

using TextureID = uint32_t;

class Texture2DManager final 
{
private:
    std::unordered_map<TextureID, std::shared_ptr<Texture2D>> m_Textures;
public:
    Texture2DManager() = default;
    static TextureID Hash(const std::string& path);

    [[nodiscard]]
    std::optional<TextureID> Load(const std::string& path);
    [[nodiscard]]
    Texture2D* Get(TextureID id);
};
