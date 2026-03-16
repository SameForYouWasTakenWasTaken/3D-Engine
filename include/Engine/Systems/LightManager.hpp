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


    template <typename T, typename... Args>
    /**
     * @brief Creates and stores a new light of type T and returns its assigned ID.
     *
     * Constructs a T with the provided constructor arguments, stores it in the manager
     * under a newly allocated LightID, and advances the internal ID counter.
     *
     * @tparam T Derived light type to construct.
     * @tparam Args Parameter pack for T's constructor.
     * @param args Arguments forwarded to T's constructor.
     * @return std::optional<LightID> The assigned LightID if the light was inserted, `std::nullopt` otherwise.
     */
    std::optional<LightID> CreateLight(Args&&... args)
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
     * @brief Retrieve a stored light by its ID and return it as the requested derived type.
     *
     * @tparam T Desired derived light type to cast to.
     * @param id Identifier of the light to retrieve.
     * @return `T*` Pointer to the light cast to `T` if the light exists and is of type `T`, `nullptr` otherwise.
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
