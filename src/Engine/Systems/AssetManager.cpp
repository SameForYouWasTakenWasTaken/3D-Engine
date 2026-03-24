#include <Engine/Systems/AssetManager.hpp>

/**
 * @brief Loads a model from the given directory into the asset cache and returns its identifier.
 *
 * If a model for the same directory has already been loaded, the existing identifier is returned
 * instead of creating a duplicate entry.
 *
 * @param directory Filesystem path or asset directory used to construct the model.
 * @return ModelID Identifier obtained by hashing the provided directory; corresponds to the cached or newly loaded model.
 */
ModelID AssetManager::Load(const std::string& directory)
{
    auto hash = Hash<ModelID>(directory);

    auto it = m_Models.find(hash);
    if (it != m_Models.end())
        return it->first;


    m_Models.emplace(hash,
                     ModelData{
                         hash,
                         directory,
                         std::make_unique<Model>(directory)
                     }
    );
    return hash;
}

/**
 * Load a model into the asset manager using the model's directory as the cache key.
 *
 * @param model Model whose directory will be used to identify and load the asset.
 * @return ModelID Identifier derived from hashing the model's directory; if the directory is already cached, returns the existing identifier.
 */
ModelID AssetManager::Load(std::unique_ptr<Model>& model)
{
    auto hash = Hash<ModelID>(model->GetDirectory());
    auto it = m_Models.find(hash);
    if (it != m_Models.end())
        return it->first;


    m_Models.emplace(hash,
                     ModelData{
                         hash,
                         model->GetDirectory(),
                         std::move(model)
                     });
    return hash;
}

/**
 * @brief Retrieve the Model corresponding to a ModelID.
 *
 * @param id Identifier of the model to retrieve.
 * @return Model* Pointer to the stored Model for `id`, or `nullptr` if no model is associated with `id`.
 */
Model* AssetManager::Get(ModelID id)
{
    auto it = m_Models.find(id);

    if (it == m_Models.end())
        return nullptr;

    return it->second.model.get();
}
