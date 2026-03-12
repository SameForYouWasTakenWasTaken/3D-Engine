#version 460 core

struct Material {
    sampler2D diffuse;
    vec3 ambient;
    vec3 specular;
    float shininess;
};

struct DirectionLight {
    vec3 color;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 color;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec4 vertexColor;
in vec2 fragmentTexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;

uniform DirectionLight dirLights[16];
uniform int numDirLights;

uniform PointLight pointLights[16];
uniform int numPointLights;

uniform vec3 viewPos;

out vec4 FragColor;

// --- Calculate directional lights from an array ---
vec3 calculateDirectional(vec3 norm, vec3 viewDir, vec3 texColor)
{
    vec3 result = vec3(0.0);

    for (int i = 0; i < numDirLights; i++)
    {
        DirectionLight light = dirLights[i];

        vec3 lightDir = normalize(-light.direction);
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

        vec3 ambient = light.ambient * texColor;
        vec3 diffuse = light.diffuse * diff * texColor;
        vec3 specular = light.specular * spec;

        result += ambient + diffuse + specular;
    }

    return result;
}

// --- Calculate point lights from an array ---
vec3 calculatePoint(vec3 norm, vec3 viewDir, vec3 fragPos, vec3 texColor)
{
    vec3 result = vec3(0.0);

    for (int i = 0; i < numPointLights; i++)
    {
        PointLight light = pointLights[i];

        vec3 lightDir = normalize(light.position - fragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant +
        light.linear * distance +
        light.quadratic * distance * distance);

        vec3 ambient = light.ambient * texColor * attenuation;
        vec3 diffuse = light.diffuse * diff * texColor * attenuation;
        vec3 specular = light.specular * spec * attenuation;

        result += ambient + diffuse + specular;
    }

    return result;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 texColor = texture(material.diffuse, fragmentTexCoord).rgb;

    // Calculate contributions per light type
    vec3 resultDirectional = calculateDirectional(norm, viewDir, texColor);
    vec3 resultPoint = calculatePoint(norm, viewDir, FragPos, texColor);

    // Combine all contributions
    vec3 result = resultDirectional + resultPoint;

    // Apply vertex color
    result *= vertexColor.rgb;

    FragColor = vec4(result, texture(material.diffuse, fragmentTexCoord).a * vertexColor.a);
}