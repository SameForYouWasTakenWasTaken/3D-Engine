#include <Engine/Systems/ShaderManager.hpp>


ShaderID ShaderManager::Load(const std::string& vert_filepath, const std::string& frag_filepath)
{
    ShaderID hash = Hash<ShaderID>(vert_filepath, frag_filepath);
    if (Get(hash)) return hash;

    auto shader = std::make_shared<Shader>(vert_filepath, frag_filepath);
    m_Shaders.emplace(
        hash,
        shader
    );
    return hash;
}

ShaderID ShaderManager::Load(const std::shared_ptr<Shader> shader)
{
    auto [vertexFilepath, fragmentFilepath] = shader->GetFilepaths();
    auto hash = Hash<ShaderID>(vertexFilepath, fragmentFilepath);

    if (Get(hash) != nullptr) return hash;

    m_Shaders.emplace(hash, shader);
    return hash;
}

std::shared_ptr<Shader> ShaderManager::Get(ShaderID id)
{
    auto it = m_Shaders.find(id);
    if (it == m_Shaders.end()) return nullptr;
    return it->second;
}
