#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

layout(location = 11) in vec4 row0;
layout(location = 12) in vec4 row1;
layout(location = 13) in vec4 row2;
layout(location = 14) in vec4 row3;

out vec4 vertexColor;
out vec2 fragmentTexCoord;

uniform mat4 PVmat; //Projection * View matrix

void main()
{
    mat4 instanceMatrix = mat4(
        row0,
        row1,
        row2,
        row3
    );
    gl_Position = PVmat * instanceMatrix * vec4(position, 1.0);
    vertexColor = color;
    fragmentTexCoord = texCoord;
}