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
    vec2 PredictedPosition;
    vec2 Velocity;
    float Density;
    float iDensity;
    float NearDensity;
    float iNearDensity;
};
struct SortedParticle
{
    uint CellIndex;
    uint pIndex;
};

// BUFFERS
layout(std430, binding = 0) buffer ParticlesBuffer
{
	Particle Particles[];
};
layout(std430, binding = 1) buffer SortedParticlesBuffer
{
	SortedParticle SortedParticles[];
};
layout(std430, binding = 2) buffer StartIndicesBuffer
{
	uint StartIndices[];
};

const uint GridLookupCount = 9;
const ivec2 GridOffsets[GridLookupCount] = ivec2[](
    ivec2(0, 0),
	ivec2(1, 0),
	ivec2(-1, 0),
	ivec2(0, 1),
	ivec2(0, -1),
	ivec2(1, 1),
	ivec2(1, -1),
	ivec2(-1, 1),
	ivec2(-1, -1)
);


uniform int particleCount;
uniform float targetDensity;
uniform float particleMass;
uniform float smoothingRadius;
uniform float borderWidth;
uniform vec2 mousePos;
uniform vec2 WorldSize;
uniform float DensityScale;

float CalcDensity(vec2 myPosition);
float DensityKernel(float dst);

ivec2 GetCellCoord(vec2 Position);
uint GetCellHash(ivec2 Coords);
uint CellIndexFromHash(uint Hash);

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

	// smoothing radius
	vec2 mouseOffset = mousePos - vs_out.WorldPos;
	if ( abs(dot(mouseOffset, mouseOffset) - smoothingRadius*smoothingRadius) <= 0.01f )
	{
		FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0);
		return;
	}

		// grid
		//vec2 normilizedPos = vs_out.WorldPos + containerSize * 0.5f;
		//normilizedPos.y = containerSize.y - normilizedPos.y;
		//float cellSize = smoothingRadius*2.0f;
		//
		//vec2 gridPos = mod(normilizedPos, cellSize);
		//float distToXLine = min(abs(gridPos.x), abs(cellSize - abs(gridPos.x)));
		//float distToYLine = min(abs(gridPos.y), abs(cellSize - abs(gridPos.y)));
		//float minDistToLine = min(distToXLine, distToYLine);
		//bool isGridPixel = minDistToLine < 0.001f * containerSize.y;
		//if ( isGridPixel )
		//{
		//	FragColor = vec4(outlineColor, 0.1);
		//	return;
		//}

		//const vec3 Hight = vec3(1.0f, 0.0f, 0.0f);
		//const vec3 Good = vec3(1.0f);
		//const vec3 Low = vec3(0.0f, 0.0f, 1.0f);
		//
		//
		//
		//const float HighDensity = targetDensity * 2.0f;
		//const float Density = CalcDensity(vs_out.WorldPos);
		//vec3 Color;
		//if ( Density < targetDensity )
		//	Color = mix(Low, Good, Density / targetDensity);
		//else
		//	Color = mix(Good, Hight, (Density-targetDensity) / (HighDensity-targetDensity));

	FragColor = vec4(vec3(0.0f), 1.0);
}

float CalcDensity(vec2 myPosition)
{
    float Density = 0.0f;

    ivec2 myCell = GetCellCoord(myPosition);
    for ( uint gr = 0; gr < GridLookupCount; ++gr )
    {
        ivec2 lookupCell = myCell + GridOffsets[gr];
        uint CellHash = GetCellHash(lookupCell);
        uint CellIndex = CellIndexFromHash(CellHash);
        for ( uint lookupIndex = StartIndices[CellIndex]; lookupIndex < particleCount; ++lookupIndex )
        {
            if ( SortedParticles[lookupIndex].CellIndex != CellIndex )
                break;
            uint otherP = SortedParticles[lookupIndex].pIndex;

            // now we got all the indices
            vec2 otherPosition = Particles[otherP].Position;
            vec2 offset = otherPosition - myPosition;
            float dst2 = dot(offset, offset);
            if ( dst2 > smoothingRadius*smoothingRadius ) continue;
            
            // now we have all the data
            float dst = sqrt(dst2);
            Density += particleMass * DensityKernel(dst);
        }
    }
    return Density;
}

float DensityKernel(float dst)
{
    float v = smoothingRadius - dst;
	return v * v * DensityScale;
}

// GRID

ivec2 GetCellCoord(vec2 Position)
{
	vec2 normilizedPos = Position + WorldSize;
	int cellX = int(floor(normilizedPos.x / smoothingRadius*2.0f));
    int cellY = int(floor(normilizedPos.y / smoothingRadius*2.0f));
	return ivec2(cellX, cellY);
}

uint GetCellHash(ivec2 Coords)
{	
	const uint PRIME1 = 0x9e3779b1;
    const uint PRIME2 = 0x85ebca6b;

	return Coords.y * PRIME1 + Coords.x * PRIME2;
}

uint CellIndexFromHash(uint Hash)
{
    return Hash % particleCount;
}