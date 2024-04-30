#version 430 core


uniform float OutlineColor;

out vec4 FragColor;


void main()
{             
    FragColor = vec4(OutlineColor, 0.0, 0.0, 1.0);
}