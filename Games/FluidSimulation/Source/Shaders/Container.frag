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
	//float cellSize = smoothingRadius*2.0f;
	//vec2 normilizedPos = vs_out.WorldPos + containerSize * 0.5f;
	//normilizedPos.y = containerSize.y - normilizedPos.y;
	//vec2 gridPos = mod(normilizedPos, cellSize);
	//float distToXLine = min(abs(gridPos.x), abs(cellSize - abs(gridPos.x)));
	//float distToYLine = min(abs(gridPos.y), abs(cellSize - abs(gridPos.y)));
	//float minDistToLine = min(distToXLine, distToYLine);
	//bool isGridPixel = minDistToLine < 0.001f * containerSize.y;
	//if ( isGridPixel )
	//{
	//	FragColor = vec4(outlineColor, 1.0);
	//	return;
	//}
	FragColor = vec4(vec3(0.0f), 1.0);
}