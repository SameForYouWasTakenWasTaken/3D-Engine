#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 aNormal;

// Move instance data to 4-10 (was 11-17)
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

uniform mat4 projectmat;
uniform mat4 viewmat;


void main()
{
    mat4 instanceMatrix = mat4(
        row0,
        row1,
        row2,
        row3
    );
    gl_Position = projectmat * viewmat * instanceMatrix * vec4(position, 1.0);
    vertexColor = color;
    fragmentTexCoord = texCoord;

    FragPos = vec3(instanceMatrix * vec4(position, 1.0));

    mat3 normalMatrix = mat3(normalRow0.xyz, normalRow1.xyz, normalRow2.xyz);
    Normal = normalize(normalMatrix * aNormal);

}