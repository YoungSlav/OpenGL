#version 430 core


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

flat in int InstanceID;
in vec2 TexCoords;

out vec4 FragColor;

uniform float maxSpeed;

uniform float TargetDensity;

const vec3 Green = vec3(0.0f, 1.0f, 0.0f);  // Low density
const vec3 Blue = vec3(0.0f, 0.0f, 1.0f);   // Good density
const vec3 Red = vec3(1.0f, 0.0f, 0.0f);    // High density

uniform vec3 Cold;
uniform vec3 Middle;
uniform vec3 Hot;

void main()
{
    if ( length(TexCoords - vec2(0.5)) > 0.5 ) discard;

    float speed = length(Particles[InstanceID].Velocity);
    const float midSpeed = maxSpeed*0.5f;
    vec3 Color;
    if ( speed < midSpeed )
        Color = mix(Cold, Middle, speed / midSpeed);
    else
        Color = mix(Middle, Hot, (speed-midSpeed) / (maxSpeed-midSpeed));

    //vec3 Color;
    //const float MaxDensity = TargetDensity * 3.0f;
    //if ( Particles[InstanceID].Density < TargetDensity )
    //    Color = mix(Blue, Green, Particles[InstanceID].Density / TargetDensity);
    //else
    //    Color = mix(Green, Red, (Particles[InstanceID].Density-TargetDensity) / (MaxDensity-TargetDensity));

    
    FragColor = vec4(Color, 1.0);
}