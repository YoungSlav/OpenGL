#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 6) in mat4 instanceMatrix;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * instanceMatrix * vec4(aPos, 1.0);
}