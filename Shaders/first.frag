#version 460 core

in vec4 vertexColor;
in vec2 fragmentTexCoord;
in vec3 Normal;
in vec3 FragPos;  

uniform sampler2D texture1;

uniform int numLights;
uniform vec3 lightPositions[16];
uniform vec3 lightColors[16];

out vec4 FragColor;

void main()
{
    float ambientStrength = 0.1;
    vec3 norm = normalize(Normal);

    vec3 totalLight = vec3(0.0);

    for (int i = 0; i < numLights; ++i)
    {
        vec3 lightDir = normalize(lightPositions[i] - FragPos); 
        float diff = max(dot(norm, lightDir), 0.0);
        totalLight += diff * lightColors[i];
    }

    vec3 ambient = ambientStrength * vec3(1.0); // or average of lights
    vec3 result = ambient + totalLight;

    vec3 texColor = texture(texture1, fragmentTexCoord).rgb;
    FragColor = vec4(result * texColor * vertexColor.rgb, 1.0);
}