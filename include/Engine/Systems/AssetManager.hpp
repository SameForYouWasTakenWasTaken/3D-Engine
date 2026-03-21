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
public:
    AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    [[nodiscard]]
    ModelID Load(const std::string& directory);

    [[nodiscard]]
    ModelID Load(const Model& model);

    [[nodiscard]]
    Model* Get(ModelID id);
};