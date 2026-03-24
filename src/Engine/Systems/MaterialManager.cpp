#include <Engine/Systems/MaterialManager.hpp>

/**
 * @brief Loads a material into the manager and returns its identifier.
 *
 * If a material with identical properties already exists in the manager, this function
 * returns the existing material's ID. Otherwise, it inserts the provided material
 * into the manager and returns the ID associated with the newly stored material.
 *
 * @param material Material to store; will be moved into the manager if inserted.
 * @return MaterialID ID of the stored material (existing or newly inserted).
 */
MaterialID MaterialManager::Load(Material material)
{
    MaterialID id = Hash<MaterialID>(
        material.shader,
        material.diffuse,
        material.specular,
        material.ambient.x,
        material.ambient.y,
        material.ambient.z,
        material.shininess,
        material.transparency
        );
    if (Get(id) != nullptr)
        return id;
    m_Materials.emplace(id, std::move(material));
    return id;
}

/**
 * @brief Creates a duplicate of an existing material and returns its new ID.
 *
 * Looks up the material identified by `id`; if present, inserts a copy under a newly allocated MaterialID and returns that ID.
 *
 * @param id Source material ID to clone.
 * @return std::optional<MaterialID> Containing the new MaterialID if the source was found, `std::nullopt` otherwise.
 */
std::optional<MaterialID> MaterialManager::Clone(MaterialID id)
{
    auto it = m_Materials.find(id);
    if (it == m_Materials.end())
        return std::nullopt;


    bool validHash = false;
    MaterialID nextID{};

    while (!validHash)
    {
        nextID = Hash<MaterialID>(id, m_NextMaterialID++);

        if (Get(nextID) == nullptr)
            validHash = true;
    }

    m_Materials.emplace(nextID, it->second);
    return nextID;
}

/**
 * @brief Retrieves a material by its identifier.
 *
 * Returns a pointer to the stored material associated with the given ID.
 *
 * @param id Identifier of the material to retrieve.
 * @return Material* Pointer to the material for `id`, or `nullptr` if no material with that ID exists.
 */
Material* MaterialManager::Get(MaterialID id) 
{ 
    auto it = m_Materials.find(id);
    if (it == m_Materials.end())
        return nullptr;
    return &it->second;
}
