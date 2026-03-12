#include <Engine/Systems/LightManager.hpp>

std::optional<LightID> LightManager::CreateLight(LightType type)
{
    LightID id = -1;
    if (type == LightType::NONE) return std::nullopt;

    LightBase base;

    if (type == LightType::DIRECTIONAL)
    {
        DirectionalLight light;
        id = AddLight(light).value();
        base.type = LightType::DIRECTIONAL;
    }

    // TODO: Make pointlight work
    if (type == LightType::POINT)
    {
        PointLight light;
        //id = AddLight(light).value();
        base.type = LightType::POINT;
    }

    m_Lights.emplace(id, base);
    return id;
}
// TODO: Function override for multiple lights
std::optional<LightID> LightManager::AddLight(DirectionalLight& light)
{
    LightID id = m_NextLightID++;
    m_DirectionalLights.emplace(id, light);

    return id;
}

void LightManager::RemoveLight(LightID id)
{
    const IsLightStr InAny = IsLight(id);

    const bool Exists = InAny.Exists;
    const LightType Type = InAny.type;

    if (!Exists) return;


    if (Type == LightType::DIRECTIONAL) m_DirectionalLights.erase(id);
    if (Type == LightType::POINT) m_PointLights.erase(id);
}

DirectionalLight* LightManager::GetDirectionalLight(LightID id)
{
    const auto it = m_DirectionalLights.find(id);

    const IsLightStr InAny = IsLight(id);

    const bool x = it == m_DirectionalLights.end();
    const bool Exists = InAny.Exists;
    const bool CorrectType = InAny.type == LightType::DIRECTIONAL;

    if (!Exists || !CorrectType) return nullptr;
    return &it->second;
}

LightType LightManager::GetLightType(LightID id)
{
    const IsLightStr LightStr = IsLight(id);
    if (!LightStr.Exists) return LightType::NONE;

    return LightStr.type;
}

LightManager::IsLightStr LightManager::IsLight(LightID id)
{
    IsLightStr hasLight; // Default has everything set to null

    const LightBase* light = GetLight(id);
    if (!light) return hasLight;

    hasLight.Exists = true;
    hasLight.type = light->type;

    return hasLight;
}


// TODO: Complete
void LightManager::UploadToShader(Shader* shader)
{   
    constexpr int MAX_LIGHTS = 16;

    int countDir = 0;
    int countPoint = 0;

    for (const auto& val : m_DirectionalLights)
    {
        if (countDir == MAX_LIGHTS) break;
        LightID id = val.first;
        DirectionalLight light = val.second;
        LightBase* base = GetLight(id);

        shader->SetVec3("dirLights[" + std::to_string(countDir) + "].color", base->color);
        shader->SetVec3("dirLights[" + std::to_string(countDir) + "].direction", light.direction);
        shader->SetVec3("dirLights[" + std::to_string(countDir) + "].ambient", light.ambient);
        shader->SetVec3("dirLights[" + std::to_string(countDir) + "].diffuse", light.diffuse);
        shader->SetVec3("dirLights[" + std::to_string(countDir) + "].specular", light.specular);

        countDir++;
    }

    // TODO: fix
    for (const auto& val : m_PointLights)
    {
        if (countPoint == MAX_LIGHTS) break;

        countPoint++;
    }

    shader->SetInt("numDirLights", countDir);
    shader->SetInt("numPointLights", countPoint);
}

LightBase* LightManager::GetLight(LightID id)
{
    const auto it = m_Lights.find(id);
    const bool IsValid = it != m_Lights.end();
    LightBase* light = &it->second;

    if (!IsValid) return nullptr;

    return light;
}
