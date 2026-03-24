#pragma once

#include <unordered_map>
#include <optional>

#include <Components/Light.hpp>
#include <Engine/LowLevel/Shader.hpp>


class LightManager final
{
    std::unordered_map<LightID, std::unique_ptr<LightBase>> m_Lights;
    LightID m_NextLightID = 0;
public:
    /**
 * @brief Constructs an empty LightManager.
 *
 * Initializes the internal light storage as empty and sets the next light ID counter to zero.
 */
LightManager() = default;
    /**
 * @brief Deleted copy constructor to prevent copying of the manager.
 *
 * Prevents copying of a LightManager to preserve unique ownership of its stored lights and avoid accidental duplication
 * or double-deletion of resources.
 */
LightManager(const LightManager&) = delete;

    template <typename T, typename... Args>
    /**
     * @brief Creates and stores a new light instance of type `T`, assigning it a unique ID.
     *
     * Constructs a `T` in place using the provided constructor arguments, stores it in the manager
     * keyed by a newly generated `LightID` (from `m_NextLightID`), and returns the assigned ID
     * when the insertion succeeds.
     *
     * @tparam T Concrete light type to construct (must derive from LightBase).
     * @tparam Args Types of constructor arguments forwarded to `T`.
     * @param args Constructor arguments forwarded to `T`'s constructor.
     * @return std::optional<LightID> `std::optional` containing the newly assigned `LightID` if the
     * insertion succeeded, `std::nullopt` if a light with the generated ID already existed.
     */
    std::optional<LightID> Load(Args&&... args)
    {
        LightID id = m_NextLightID++;
        const auto [it, inserted] = m_Lights.try_emplace(
            id,
            std::make_unique<T>(std::forward<Args>(args)...)
        );

        if (inserted)
        {
            return id;
        }

        return std::nullopt;
    }

    template <typename T>
    /**
     * @brief Retrieve a stored light by ID cast to the requested derived light type.
     *
     * @tparam T Desired light type to retrieve (must derive from LightBase).
     * @param id Identifier of the light to look up.
     * @return T* Pointer to the light cast to `T` if a light with `id` exists and is of type `T`, `nullptr` otherwise.
     */
    T* GetLight(LightID id)
    {
        auto it = m_Lights.find(id);
        const bool exists = it != m_Lights.end();

        if (exists)
        {
            LightBase* light = it->second.get();
            return dynamic_cast<T*>(light);
        }

        return nullptr;
    }

    void RemoveLight(LightID id);
    void UploadToShader(Shader* shader);

};
