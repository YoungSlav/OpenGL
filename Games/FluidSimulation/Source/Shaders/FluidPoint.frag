#version 430 core

struct Particle
{
    vec2 Position;
    vec2 Velocity;
	int Highlight;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
	Particle particles[];
};

flat in int InstanceID;
in vec2 TexCoords;

out vec4 FragColor;

uniform float maxSpeed;

uniform vec3 Cold;
uniform vec3 Middle;
uniform vec3 Hot;

const vec3 HighlightColor = vec3(1.0f, 0.0f, 0.0f);

void main()
{
    if ( length(TexCoords - vec2(0.5)) > 0.5 ) discard;

    float speed = length(particles[InstanceID].Velocity);
    const float midSpeed = maxSpeed*0.5f;
    vec3 Color;
    if ( speed < midSpeed )
        Color = mix(Cold, Middle, speed / midSpeed);
    else
        Color = mix(Middle, Hot, (speed-midSpeed) / (maxSpeed-midSpeed));

    
    FragColor = vec4(particles[InstanceID].Highlight == 0 ? Color : HighlightColor, 1.0);
}