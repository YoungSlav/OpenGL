#version 430 core

uniform vec2 containerSize;
uniform vec3 outlineColor;

in VS_OUT {
	vec2 TexCoords;
	vec2 WorldPos;
} vs_out;


out vec4 FragColor;

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

uniform int particleCount;
uniform float targetDensity;
uniform float particleMass;
uniform float smoothingRadius;
uniform float borderWidth;



const float pi = 3.14159265359f;

float SmoothingKernel(float radius, float distance)
{
	if ( distance >= radius ) return 0.0f;
	distance = distance / radius;
	radius = 1.0f;

	float volume = (315.0f / (64.0f * pi)) * pow(smoothingRadius, 9);
	float value = (radius * radius - distance * distance);
	return value * value * value / volume;
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
	// border
	vec2 borderWidthUV = vec2(borderWidth) / containerSize;
	vec2 distToCenter = abs(vs_out.TexCoords - vec2(0.5)) * 2.0f;
	vec2 summ = distToCenter + borderWidthUV;
	bool isBorder = summ.x >= 1.0 || summ.y >= 1.0;
	if ( isBorder )
	{
		FragColor = vec4(outlineColor, 1.0);
		return;
	}

	// grid
	float cellSize = smoothingRadius*2.0f;
	vec2 normilizedPos = vs_out.WorldPos + containerSize * 0.5f;
	normilizedPos.y = containerSize.y - normilizedPos.y;
	vec2 gridPos = mod(normilizedPos, cellSize);
	float distToXLine = min(abs(gridPos.x), abs(cellSize - abs(gridPos.x)));
	float distToYLine = min(abs(gridPos.y), abs(cellSize - abs(gridPos.y)));
	float minDistToLine = min(distToXLine, distToYLine);
	bool isGridPixel = minDistToLine < 0.001f * containerSize.y;
	if ( isGridPixel )
	{
		FragColor = vec4(outlineColor, 1.0);
		return;
	}

	normilizedPos -= containerSize * 0.5f;
	float density = CalcDensity(normilizedPos);
	float minDensity = 0.0f;
	float maxDensity = targetDensity * 2.0f;


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
}