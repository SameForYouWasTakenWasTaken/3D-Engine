#pragma once

#include <unordered_map>

#include <Components/Light.hpp>
#include <Engine/LowLevel/Shader.hpp>

class LightManager final
{
    std::unordered_map<LightID, Light> m_Lights;
    LightID m_NextLightID = 0;
public:

    LightID CreateLight();
    LightID AddLight(Light light);

    void RemoveLight(LightID id);

    void UpdateToShader(Shader* shader) const;
    Light* GetLight(LightID id);
};