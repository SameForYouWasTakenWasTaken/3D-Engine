#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 aNormal;

layout(location = 4) in vec4 row0;
layout(location = 5) in vec4 row1;
layout(location = 6) in vec4 row2;
layout(location = 7) in vec4 row3;

layout(location = 8) in vec4 normalRow0;
layout(location = 9) in vec4 normalRow1;
layout(location = 10) in vec4 normalRow2;

out vec4 vertexColor;
out vec2 fragmentTexCoord;
out vec3 FragPos;
out vec3 Normal;

layout(std140, binding = 0) uniform CameraBlock
{
    mat4 projectmat;
    mat4 viewmat;
    vec3 viewPos;
    float nearPlane;
    float farPlane;
    vec3 _cameraPad;
};

void main()
{
    mat4 model = mat4(row0, row1, row2, row3);
    mat3 normalMatrix = mat3(normalRow0.xyz, normalRow1.xyz, normalRow2.xyz);

    vec4 worldPos = model * vec4(position, 1.0);

    FragPos = worldPos.xyz;
    Normal = normalize(normalMatrix * aNormal);
    vertexColor = color;
    fragmentTexCoord = texCoord;

    gl_Position = projectmat * viewmat * worldPos;
}