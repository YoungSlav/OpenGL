#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 6) in mat4 instanceMatrix;

uniform mat4 model;

void main()
{
    gl_Position = model * instanceMatrix * vec4(aPos, 1.0);
}