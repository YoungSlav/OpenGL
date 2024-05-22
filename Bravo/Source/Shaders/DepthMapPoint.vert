#version 430 core
layout (location = 0) in vec3 aPos;

struct InstanceData
{
	mat4 transform;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
	InstanceData instances[];
};


uniform mat4 model;

void main()
{
	gl_Position = model * instances[gl_InstanceID].transform * vec4(aPos, 1.0);
} 