#version 430 core
layout (location = 0) in vec3 aPos;

layout (location = 6) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}