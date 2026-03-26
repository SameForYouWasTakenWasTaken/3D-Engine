#version 460 core

struct Material {
    sampler2D diffuse;
    vec3 ambient;
    vec3 specular;
    float shininess;
    float transparency;
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

struct SpotLight {
    vec3 color;
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;
    float dist;

    // point light variables
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Camera
{
    vec3 viewPos;
    float near;
    float far;
};

in vec4 vertexColor;
in vec2 fragmentTexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform Camera cam;

uniform Material material;

// numDirLights is configured in LightManager UploadShader, same for numPointLights
uniform DirectionLight dirLights[16];
uniform int numDirLights;

uniform PointLight pointLights[16];
uniform int numPointLights;

uniform SpotLight spotLights[16];
uniform int numSpotLights;

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

        vec3 ambient  = light.ambient * texColor;
        vec3 diffuse  = light.diffuse * diff * texColor * attenuation;
        vec3 specular = light.specular * spec * attenuation;

        result += ambient + diffuse + specular;
    }

    return result;
}

vec3 calculateSpot(vec3 norm, vec3 viewDir, vec3 fragPos, vec3 texColor)
{
    vec3 result = vec3(0.0);

    for (int i = 0; i < numSpotLights; i++)
    {
        SpotLight light = spotLights[i];


        float distance = length(light.position - fragPos);


        if(distance > light.dist)
            continue;


        vec3 lightDir = normalize(light.position - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        float diff = max(dot(norm, lightDir), 0.0);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        float attenuation = 1.0 / (light.constant +
        light.linear * distance +
        light.quadratic * distance * distance);

        float theta = dot(lightDir, normalize(-light.direction));

        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 ambient  = light.ambient * texColor;
        vec3 diffuse  = light.diffuse * diff * texColor;
        vec3 specular = light.specular * spec;

        result += (ambient + diffuse + specular) * attenuation * intensity;
    }

    return result;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cam.viewPos - FragPos);
    vec3 texColor = texture(material.diffuse, fragmentTexCoord).rgb;

    // Calculate contributions per light type
    vec3 resultDirectional = calculateDirectional(norm, viewDir, texColor);
    vec3 resultPoint = calculatePoint(norm, viewDir, FragPos, texColor);
    vec3 resultSpot = calculateSpot(norm, viewDir, FragPos, texColor);

    // Combine all contributions
    vec3 result = resultDirectional + resultPoint + resultSpot;

    // Apply vertex color
    result *= vertexColor.rgb;

    float alpha = 1;
    alpha *= texture(material.diffuse, fragmentTexCoord).a;
    alpha *= vertexColor.a;
    alpha *= material.transparency;

    FragColor = vec4(result, alpha);
}

