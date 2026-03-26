#include "ShaderManager.hpp"


/**
 * @brief Load a shader from vertex and fragment file paths into the manager and obtain its identifier.
 *
 * Loads the shader identified by the provided vertex and fragment file paths into the internal shader map.
 *
 * @param vert_filepath Path to the vertex shader source file.
 * @param frag_filepath Path to the fragment shader source file.
 * @return ShaderID The identifier for the loaded shader; if a shader for the given file paths already exists, returns the existing identifier.
 */
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

/**
 * @brief Adds the given shader to the manager if a shader with the same filepaths is not already present.
 *
 * Computes an identifier from the shader's vertex and fragment filepaths and inserts the provided
 * shared shader into the internal map when no shader with that identifier exists.
 *
 * @param shader Shared pointer to the Shader whose filepaths are used to compute the identifier.
 * @return ShaderID The computed identifier for the shader; if a shader with the same identifier already existed,
 * the existing identifier is returned without modifying the manager.
 */
ShaderID ShaderManager::Load(const std::shared_ptr<Shader>& shader)
{
    auto vf = shader->GetFilepaths();
    ShaderID id = Hash<ShaderID>(vf.first, vf.second);
    if (m_Shaders.contains(id)) return id;

    auto [vertexFilepath, fragmentFilepath] = shader->GetFilepaths();
    auto hash = Hash<ShaderID>(vertexFilepath, fragmentFilepath);

    if (Get(hash) != nullptr) return hash;

    m_Shaders.emplace(hash, shader);
    return hash;
}

/**
 * @brief Retrieve the shader associated with the given ShaderID.
 *
 * @param id Identifier for the shader (computed from vertex and fragment file paths).
 * @return std::shared_ptr<Shader> Shared pointer to the shader if present, `nullptr` if no shader is registered for `id`.
 */
std::shared_ptr<Shader> ShaderManager::Get(ShaderID id)
{
    auto it = m_Shaders.find(id);
    if (it == m_Shaders.end()) return nullptr;
    return it->second;
}
