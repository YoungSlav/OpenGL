#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;

struct InstanceData
{
	mat4 transform;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
	InstanceData instances[];
};


void main()
{
    gl_Position = transform * instances[gl_InstanceID].transform * vec4(aPos, 1.0);
}