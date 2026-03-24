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
    /**
 * @brief Default-constructs the ShaderManager service.
 *
 * Initializes the manager with no loaded shaders.
 */
ShaderManager() = default;
    /**
 * @brief Copy construction is disabled for ShaderManager.
 *
 * Prevents creating a copy of a ShaderManager instance to preserve its internal shader ownership and service identity.
 */
ShaderManager(const ShaderManager&) = delete;

    [[nodiscard]]
    ShaderID Load(const std::string& vert, const std::string& frag);

    [[nodiscard]]
    ShaderID Load(std::shared_ptr<Shader> shader);

    [[nodiscard]]
    std::shared_ptr<Shader> Get(ShaderID id);
};