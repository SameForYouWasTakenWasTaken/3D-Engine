#pragma once

#include <unordered_map>
#include <optional>

#include <Components/DirectionalLight.hpp>
#include <Engine/LowLevel/Shader.hpp>


class LightManager final
{
    std::unordered_map<LightID, LightBase> m_Lights;
    std::unordered_map<LightID, DirectionalLight> m_DirectionalLights;
    std::unordered_map<LightID, PointLight> m_PointLights;

    LightID m_NextLightID = 0;

    struct IsLightStr
    {
        bool Exists = false;
        LightType type = LightType::NONE;
    };
public:

    std::optional<LightID> CreateLight(LightType type);

    std::optional<LightID> AddLight(DirectionalLight& light);
    std::optional<LightID> AddLight(PointLight light);

    void RemoveLight(LightID id);

    void UploadToShader(Shader* shader);

    LightBase* GetLight(LightID id);

    DirectionalLight* GetDirectionalLight(LightID id);
    PointLight* GetPointLight(LightID id);

    LightType GetLightType(LightID id);
    IsLightStr IsLight(LightID id);
};