#include "Light.hpp"

/**
 * @brief Get the light type for this instance.
 *
 * @return LightType::POINT
 */
LightType PointLight::GetType() const
{return LightType::POINT;}

/**
 * @brief Identifies the light as a directional light.
 *
 * @return LightType::DIRECTIONAL The enum value representing a directional light.
 */
LightType DirectionalLight::GetType() const
{return LightType::DIRECTIONAL;}

/**
 * @brief Identifies this light as a spot light.
 *
 * @return LightType::SPOT The spot light type.
 */
LightType SpotLight::GetType() const
{return LightType::SPOT;}