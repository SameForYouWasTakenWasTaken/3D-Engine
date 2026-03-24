#pragma once

#include <App/Services.hpp>
#include <Engine/Systems/ShaderManager.hpp>
#include <Components/Material.hpp>

class MaterialManager : public IService
{
    std::unordered_map<MaterialID, Material> m_Materials;

    uint32_t m_NextMaterialID = 0;
public:
    /**
 * @brief Constructs an empty MaterialManager.
 *
 * Initializes the manager with an empty material map and sets the next
 * material ID counter to 0.
 */
MaterialManager() = default;
    /**
 * @brief Prevents copying of MaterialManager instances.
 *
 * The copy constructor is deleted to enforce unique ownership semantics; MaterialManager objects cannot be copied.
 */
MaterialManager(const MaterialManager&) = delete;
    [[nodiscard]]
    MaterialID Load(Material material);

    [[nodiscard]]
    std::optional<MaterialID> Clone(MaterialID id);

    [[nodiscard]]
    Material* Get(MaterialID id);
};