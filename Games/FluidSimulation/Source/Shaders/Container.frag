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
	//vec2 containerCoords = (TexCoords) * containerSize*2.0f;
	//vec2 worldPos = containerCoords * 1.0f;
	//
	//float cellSize = smoothingRadius*2.0f;
	//
    //vec2 gridPos = mod(worldPos, cellSize) - vec2(cellSize);
    //
    //// Calculate the distance to the nearest grid line in both x and y directions
    //float distToXLine = min(abs(gridPos.x), abs(cellSize - abs(gridPos.x)));
    //float distToYLine = min(abs(gridPos.y), abs(cellSize - abs(gridPos.y)));
    //
    //// Find the minimum distance to either the vertical or horizontal grid line
    //float minDistToLine = min(distToXLine, distToYLine);
	//
    //// Determine if the pixel is on a grid line
    //bool isGridPixel = minDistToLine < 0.1 * 0.5;
	//
	//if ( isGridPixel )
	//{
	//	//FragColor = vec4(outlineColor, 1.0);
	//	//return;
	//}
	
	
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
	vec2 gridPos = mod(normilizedPos, cellSize);
	float distToXLine = min(abs(gridPos.x), abs(cellSize - abs(gridPos.x)));
	float distToYLine = min(abs(gridPos.y), abs(cellSize - abs(gridPos.y)));
	float minDistToLine = min(distToXLine, distToYLine);
	bool isGridPixel = minDistToLine < 0.1 * 0.5;
	if ( isGridPixel )
	{
		FragColor = vec4(outlineColor, 1.0);
		return;
	}


	//else
	//{
	//	float density = CalcDensity(worldPos);
	//	float minDensity = 0.0f;
	//	float maxDensity = targetDensity * 4.0f;
	//
	//	vec3 Color;
	//	if ( density < targetDensity )
	//	{
	//		float alpha = (density - minDensity) / (targetDensity - minDensity);
	//		Color = mix(Cold, Middle, alpha);
	//	}
	//	else
	//	{
	//		float alpha = (density - targetDensity) / (maxDensity - targetDensity);
	//		Color = mix(Middle, Hot, alpha);
	//	}
	//
	//	FragColor = vec4(Color, 1.0);
	//	return;
	//}
}