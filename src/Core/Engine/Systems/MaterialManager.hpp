#pragma once

#include <optional>
#include <../../../src/Core/Engine/Systems/Services.hpp>
#include <../../../src/Core/Engine/Systems/ShaderManager.hpp>
#include <../../../src/Core/Engine/Components/Material.hpp>

class MaterialManager : public IService
{
    std::unordered_map<MaterialID, Material> m_Materials;

    MaterialID m_NextMaterialID = 0;
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