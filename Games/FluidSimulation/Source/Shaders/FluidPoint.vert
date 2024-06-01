#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

struct Particle
{
    vec2 Position;
    vec2 Velocity;
    vec2 Acceleration;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
	Particle particles[];
};

uniform mat4 viewProj;
uniform float particleSize;

flat out int InstanceID;
out vec2 TexCoords;


void main()
{
    InstanceID = gl_InstanceID;
    TexCoords = aTexCoords;
    vec3 scaledVertex = vec3(aPos * particleSize, 0.2);
    vec3 tranformedVertex = vec3(particles[gl_InstanceID].Position, 0.0) + scaledVertex;
    gl_Position = viewProj * vec4(tranformedVertex, 1.0);
}