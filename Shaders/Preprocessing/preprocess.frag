#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_ScreenTexture;

void main()
{
    FragColor = texture(u_ScreenTexture, TexCoord);
}