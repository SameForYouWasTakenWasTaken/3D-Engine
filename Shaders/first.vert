#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 aNormal;

layout(location = 11) in vec4 row0;
layout(location = 12) in vec4 row1;
layout(location = 13) in vec4 row2;
layout(location = 14) in vec4 row3;

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
    Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
}