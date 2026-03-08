#include <Engine/Systems/ShaderManager.hpp>

ShaderID ShaderManager::Hash(const std::string& vert, const std::string& frag)
{
    size_t h1 = std::hash<std::string>{}(vert);
    size_t h2 = std::hash<std::string>{}(frag);

    return h1 ^ (h2 << 1);
}

ShaderID ShaderManager::Load(const std::string& vert_filepath, const std::string& frag_filepath)
{
    ShaderID id = Hash(vert_filepath, frag_filepath);
    if (m_Shaders.find(id) != m_Shaders.end()) return id;

    m_Shaders[id] = std::make_shared<Shader>(vert_filepath, frag_filepath);
    return id;
}

Shader* ShaderManager::Get(ShaderID id) { 
    if (m_Shaders.find(id) == m_Shaders.end()) return nullptr;
    return m_Shaders[id].get(); 
}