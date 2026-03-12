#version 460 core

struct Material {
    sampler2D diffuse;   // diffuse map
    vec3 ambient;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;      // base color of the light
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec4 vertexColor;
in vec2 fragmentTexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform Light lights[16]; // array of lights
uniform int numLights;    // number of active lights
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    // --- Normalize vectors ---
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // --- Sample diffuse map ---
    vec3 texColor = texture(material.diffuse, fragmentTexCoord).rgb;

    // --- Accumulators ---
    vec3 ambientAccum = vec3(0.0);
    vec3 diffuseAccum = vec3(0.0);
    vec3 specularAccum = vec3(0.0);

    // --- Loop over lights ---
    for (int i = 0; i < numLights; ++i)
    {
        Light currLight = lights[i];

        // Ambient contribution
        ambientAccum += currLight.ambient * currLight.color * texColor;

        // Diffuse contribution (Lambert)
        vec3 lightDir = normalize(currLight.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuseAccum += diff * currLight.diffuse * currLight.color * texColor;

        // Specular contribution (Phong)
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specularAccum += spec * currLight.specular * currLight.color;
    }

    // --- Combine results ---
    vec3 result = ambientAccum + diffuseAccum + specularAccum;

    // Apply vertex color
    result *= vertexColor.rgb;

    // Final fragment color
    FragColor = vec4(result, texture(material.diffuse, fragmentTexCoord).a * vertexColor.a);
}