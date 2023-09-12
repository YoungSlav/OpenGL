#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 6) in mat4 instanceMatrix;

uniform mat4 transform;

flat out float InstanceID;

void main()
{
	InstanceID = gl_InstanceID;
    gl_Position = transform * instanceMatrix * vec4(aPos, 1.0);
}