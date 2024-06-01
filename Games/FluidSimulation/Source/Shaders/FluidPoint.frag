#version 430 core

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

flat in int InstanceID;
in vec2 TexCoords;

out vec4 FragColor;

const vec3 Cold = vec3(0.0f, 0.0f, 1.0f);
const vec3 Hot = vec3(1.0f, 0.0f, 0.0f);

uniform float particleSize;

void main()
{
    if ( length(TexCoords - vec2(0.5)) > 0.5 ) discard;

    vec3 Color = mix(Cold, Hot, length(particles[InstanceID].Velocity));
    
    FragColor = vec4(Color, 1.0);
}