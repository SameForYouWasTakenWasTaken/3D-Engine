#version 460 core

in vec4 vertexColor;
in vec2 fragmentTexCoord;

uniform sampler2D texture1;

out vec4 FragColor;

uniform sampler2D ShaderTexture;

void main()
{
    //vec4 texColor = texture(texture1, fragmentTexCoord);
    // FragColor = texColor * vec4(vertexColor, 1.0);
    FragColor = vertexColor * texture(texture1, fragmentTexCoord);
}