#pragma once

#include <unordered_map>

#include <Components/Light.hpp>
#include <Engine/LowLevel/Shader.hpp>

class LightManager final
{
    std::unordered_map<uint32_t, COMPLight> m_Lights;
    uint32_t m_NextLightID = 0;
public:

    uint32_t CreateLight();
    uint32_t AddLight(COMPLight light);
    void RemoveLight(uint32_t id);

    void UpdateToShader(Shader* shader);
    COMPLight* GetLight(uint32_t id);
};