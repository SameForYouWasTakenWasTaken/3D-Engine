#include <Engine/Systems/AssetManager.hpp>

ModelID AssetManager::Load(const std::string& directory)
{
    auto hash = Hash<ModelID>(directory);

    auto it = m_Models.find(hash);
    if (it != m_Models.end())
        return it->first;

    ModelData data{
        hash,
        directory,
        Model(directory)
    };


    m_Models.emplace(hash, data);
    return hash;
}

ModelID AssetManager::Load(const Model& model)
{
    const auto dir = model.GetDirectory();
    auto hash = Hash<ModelID>(dir);

    auto it = m_Models.find(hash);
    if (it != m_Models.end())
        return it->first;


    m_Models.emplace(hash,
        ModelData{
        hash,
        dir,
        Model(dir)
    });
    return hash;
}

Model* AssetManager::Get(ModelID id)
{
    auto it = m_Models.find(id);

    if (it == m_Models.end())
        return nullptr;

    return &it->second.model;
}
