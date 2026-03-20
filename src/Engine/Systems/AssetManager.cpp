#include <Engine/Systems/AssetManager.hpp>

ModelID AssetManager::Load(const std::string& directory)
{
    std::string key = directory;

    if (m_PathToID.contains(key))
        return m_PathToID[key];

    auto id = m_NextModelID++;

    ModelData data{
        id,
        directory,
        Model(directory)
    };


    m_Models.emplace(id, data);
    m_PathToID.emplace(directory, id);
    return id;
}

Model* AssetManager::Get(ModelID id)
{
    auto it = m_Models.find(id);

    if (it == m_Models.end())
        return nullptr;

    return &it->second.model;
}
