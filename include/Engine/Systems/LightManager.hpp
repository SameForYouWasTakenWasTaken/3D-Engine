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
