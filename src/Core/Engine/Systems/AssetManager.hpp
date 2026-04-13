#pragma once

#include <unordered_map>

#include "Engine/Systems/Services.hpp"
#include "Engine/Components/Model.hpp"


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
    /**
 * @brief Constructs an AssetManager with no loaded models.
 *
 * Initializes the manager to its default state with an empty model storage.
 */
AssetManager() = default;
    /**
 * @brief Disabled copy constructor; AssetManager instances cannot be copy-constructed.
 *
 * Prevents accidental copying of the service and its internal model storage.
 */
AssetManager(const AssetManager&) = delete;
    [[nodiscard]]
    ModelID Load(const std::string& directory);

    [[nodiscard]]
    ModelID Load(Model& model);

    [[nodiscard]]
    Model* Get(ModelID id);
};