#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

struct Particle
{
    vec2 Position;
    vec2 PredictedPosition;
    vec2 Velocity;
    float Density;
    float NearDensity;
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
    vec3 scaledVertex = vec3(aPos * particleSize*2.0f, 0.2);
    vec3 particlePosition = vec3(particles[gl_InstanceID].Position, 0.0);
    particlePosition.y *= -1.0f;
    vec3 tranformedVertex = particlePosition + scaledVertex;
    gl_Position = viewProj * vec4(tranformedVertex, 1.0);
}