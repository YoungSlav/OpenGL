#version 430 core

uniform vec2 containerSize;
uniform vec3 outlineColor;

in vec2 TexCoords;


out vec4 FragColor;

struct Particle
{
    vec2 Position;
    vec2 Velocity;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
	Particle particles[];
};

uniform int particleCount;
uniform float targetDensity;
uniform float particleMass;
uniform float smoothingRadius;
uniform float borderWidth;



const float pi = 3.14159265359f;

float SmoothingKernel(float radius, float distance)
{
	if ( distance >= radius ) return 0.0f;

	float volume = pi * pow(radius, 4) / 6.0f;
	return (radius - distance) * (radius - distance) / volume;
}

float CalcDensity(vec2 samplePoint)
{
	float density = 0.0f;

	for ( int i = 0; i < particleCount; i++ )
	{
		float dst = length(particles[i].Position - samplePoint);
		float influence = SmoothingKernel(smoothingRadius, dst);
		density += particleMass * influence;
	}
	return density;
}

const vec3 Cold = vec3(0.0f, 0.0f, 1.0f);
const vec3 Middle = vec3(1.0f, 1.0f, 1.0f);
const vec3 Hot = vec3(1.0f, 0.0f, 0.0f);


void main()
{
	vec2 borderWidthUV = vec2(borderWidth*2.0) / containerSize;

	
	vec2 distToCenter = abs(TexCoords - vec2(0.5)) * 2.0;
	vec2 summ = distToCenter + borderWidthUV;
	bool isBorder = summ.x >= 1.0 || summ.y >= 1.0;
	if ( isBorder )
	{
		FragColor = vec4(outlineColor, 1.0);
		return;
	}
	else
	{
		vec2 containerCoords = (TexCoords - vec2(0.5)) * containerSize;
		vec2 worldPos = containerCoords * -1.0f;
		

		float density = CalcDensity(worldPos);
		float minDensity = 0.0f;
		float maxDensity = targetDensity * 4.0f;

		vec3 Color;
		if ( density < targetDensity )
		{
			float alpha = (density - minDensity) / (targetDensity - minDensity);
			Color = mix(Cold, Middle, alpha);
		}
		else
		{
			float alpha = (density - targetDensity) / (maxDensity - targetDensity);
			Color = mix(Middle, Hot, alpha);
		}

		FragColor = vec4(Color, 1.0);
		return;
	}
}