#version 460 core

struct DirectionLight
{
    vec3 color;
    float intensity;
    vec3 direction; float _pad1;
    vec3 ambient;   float _pad2;
    vec3 diffuse;   float _pad3;
    vec3 specular;  float _pad4;
};

struct PointLight
{
    vec3 color;
    float intensity;
    vec3 position; float _pad1;

    float constant;
    float linear;
    float quadratic;
    float _pad2;

    vec3 ambient;  float _pad3;
    vec3 diffuse;  float _pad4;
    vec3 specular; float _pad5;
};

struct SpotLight
{
    vec3 color;
    float intensity;
    vec3 position;  float _pad1;
    vec3 direction; float _pad2;

    float cutOff;
    float outerCutOff;
    float dist;
    float _pad3;

    float constant;
    float linear;
    float quadratic;
    float _pad4;

    vec3 ambient;   float _pad5;
    vec3 diffuse;   float _pad6;
    vec3 specular;  float _pad7;
};

in vec4 vertexColor;
in vec2 fragmentTexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

// Camera data can stay in a UBO
layout(std140, binding = 0) uniform CameraBlock
{
    mat4 projectmat;
    mat4 viewmat;
    vec3 viewPos;
    float nearPlane;
    float farPlane;
    vec3 _cameraPad;
};

// Material constants can stay in a UBO
layout(std140, binding = 1) uniform MaterialBlock
{
    vec3 ambient;
    float shininess;
    float transparency;
};

// Texture must stay separate
uniform sampler2D materialDiffuse;
uniform sampler2D materialSpecular;

// Counts stay as plain uniforms
uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;

// SSBOs
layout(std430, binding = 2) readonly buffer DirectionLightBuffer
{
    DirectionLight dirLights[];
};

layout(std430, binding = 3) readonly buffer PointLightBuffer
{
    PointLight pointLights[];
};

layout(std430, binding = 4) readonly buffer SpotLightBuffer
{
    SpotLight spotLights[];
};

// --- Calculate directional lights from an array ---
vec3 calculateDirectional(vec3 norm, vec3 viewDir, vec3 specular, vec3 texColor)
{
    vec3 result = vec3(0.0);
    for (int i = 0; i < numDirLights; i++)
    {
        DirectionLight light = dirLights[i];

        vec3 lightDir = normalize(-light.direction);
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        vec3 ambientTerm  = light.ambient * ambient * texColor;
        vec3 diffuseTerm  = light.diffuse * diff * texColor;
        vec3 specularTerm = light.specular * spec * specular;

        result += (ambientTerm + diffuseTerm + specularTerm) * light.color * light.intensity;
    }

    return result;
}

// --- Calculate point lights from an array ---
vec3 calculatePoint(vec3 norm, vec3 viewDir, vec3 fragPos, vec3 specular, vec3 texColor)
{
    vec3 result = vec3(0.0);
    for (int i = 0; i < numPointLights; i++)
    {
        PointLight light = pointLights[i];

        vec3 lightDir = normalize(light.position - fragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
        );

        vec3 ambientTerm  = light.ambient * ambient * texColor;
        vec3 diffuseTerm  = light.diffuse * diff * texColor;
        vec3 specularTerm = light.specular * spec * specular;

        result += (ambientTerm + diffuseTerm + specularTerm) * attenuation * light.color * light.intensity;
    }

    return result;
}

vec3 calculateSpot(vec3 norm, vec3 viewDir, vec3 fragPos, vec3 specular, vec3 texColor)
{
    vec3 result = vec3(0.0);
    for (int i = 0; i < numSpotLights; i++)
    {
        SpotLight light = spotLights[i];

        float distance = length(light.position - fragPos);

        if (distance > light.dist)
        continue;

        vec3 lightDir = normalize(light.position - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        float diff = max(dot(norm, lightDir), 0.0);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
        );

        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float spotFactor = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 ambientTerm  = light.ambient * ambient * texColor;
        vec3 diffuseTerm  = light.diffuse * diff * texColor;
        vec3 specularTerm = light.specular * spec * specular;

        result += (ambientTerm + diffuseTerm + specularTerm)
        * light.color
        * light.intensity
        * attenuation
        * spotFactor;
    }

    return result;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 texSample = texture(materialDiffuse, fragmentTexCoord);
    vec3 specular = texture(materialSpecular, fragmentTexCoord).rgb;
    vec3 texColor = texSample.rgb;

    vec3 resultDirectional = calculateDirectional(norm, viewDir, specular, texColor);
    vec3 resultPoint = calculatePoint(norm, viewDir, FragPos, specular, texColor);
    vec3 resultSpot = calculateSpot(norm, viewDir, FragPos, specular, texColor);

    vec3 result = resultDirectional + resultPoint + resultSpot;

    result *= vertexColor.rgb;

    float alpha = 1.0;
    alpha *= texSample.a;
    alpha *= vertexColor.a;
    alpha *= transparency;

    FragColor = vec4(result, alpha);
}