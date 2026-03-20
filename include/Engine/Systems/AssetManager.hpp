#pragma once

#include <unordered_map>
#include <App/Services.hpp>
#include <Components/Model.hpp>


struct ModelData
{
    ModelID id;
    std::string directory;
    Model model;
};
class AssetManager : public IService
{
    std::unordered_map<ModelID, ModelData> m_Models;
    std::unordered_map<std::string, ModelID> m_PathToID;

    ModelID m_NextModelID = 0;
public:
    ModelID Load(const std::string& directory);
    Model* Get(ModelID id);
};