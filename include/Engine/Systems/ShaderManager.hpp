#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include <App/Services.hpp>
#include <Engine/LowLevel/Shader.hpp>
#include <Components/Material.hpp> // For ShaderID val

class ShaderManager : public IService
{
    std::unordered_map<ShaderID, std::shared_ptr<Shader>> m_Shaders;
public:


    [[nodiscard]]
    ShaderID Load(const std::string& vert, const std::string& frag);

    [[nodiscard]]
    ShaderID Load(std::shared_ptr<Shader> shader);

    [[nodiscard]]
    std::shared_ptr<Shader> Get(ShaderID id);
};