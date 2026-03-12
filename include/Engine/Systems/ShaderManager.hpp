#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include <Engine/LowLevel/Shader.hpp>
#include <Components/Material.hpp> // For ShaderID val

class ShaderManager final 
{
private:
    std::unordered_map<ShaderID, std::shared_ptr<Shader>> m_Shaders;

public:
    
    static ShaderID Hash(const std::string& vert, const std::string& frag);
    
    [[nodiscard]]
    ShaderID Load(const std::string& vert, const std::string& frag);
    
    [[nodiscard]]
    Shader* Get(ShaderID id);
};