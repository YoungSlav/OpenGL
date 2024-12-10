#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

struct Particle
{
    vec3 Position;
    vec3 PredictedPosition;
    vec3 Velocity;
    float Density;
    float iDensity;
};

layout(std430, binding = 0) buffer ParticlesBuffer
{
	Particle Particles[];
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 viewProj;
uniform float particleSize;

flat out int InstanceID;
out vec2 TexCoords;


void main()
{
    InstanceID = gl_InstanceID;
    TexCoords = aTexCoords;

    // Particle center position
    vec3 particlePosition = Particles[gl_InstanceID].Position;

    // Extract the right and up vectors from the view matrix
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    // Transform the quad vertices into world space using the camera's orientation
    vec3 scaledRight = aPos.x * particleSize * cameraRight;
    vec3 scaledUp = aPos.y * particleSize * cameraUp;

    // Compute the final vertex position
    vec3 billboardVertex = particlePosition + scaledRight + scaledUp;

    // Transform to clip space
    gl_Position = viewProj * vec4(billboardVertex, 1.0);
}