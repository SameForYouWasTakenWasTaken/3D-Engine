#pragma once
#include <unordered_map>
#include <memory>

#include <Engine/LowLevel/Texture2D.hpp>

using TextureID = uint32_t;

class Texture2DManager final 
{
private:
    std::unordered_map<TextureID, std::shared_ptr<Texture2D>> m_Textures;
    uint32_t m_NextTextureID = 0;

public:
    Texture2DManager() = default;
    static TextureID Hash(const std::string& path);

    [[nodiscard]]
    TextureID Load(const std::string& path);
    [[nodiscard]]
    Texture2D* Get(TextureID id);
};
