#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;


void main()
{
    // TODO
    FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);
} 