#version 430 core

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

uniform float DensityScale;
uniform int ParticleCount;
uniform float SmoothingRadius;
uniform float ParticleMass;
uniform vec2 WorldSize;

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

float CalcDensity(vec2 myPosition);
float DensityKernel(float dst);

ivec2 GetCellCoord(vec2 Position);
uint GetCellHash(ivec2 Coords);
uint CellIndexFromHash(uint Hash);


void main()
{
    float Density = CalcDensity(vs_out.WorldPos);
    FragColor = vec4(Density, Density, Density, 1.0);
}

float CalcDensity(vec2 position)
{
    float Density = 0.0f;
    float NearDensity = 0.0f;

    ivec2 myCell = GetCellCoord(position);
    for ( uint gr = 0; gr < GridLookupCount; ++gr )
    {
        ivec2 lookupCell = myCell + GridOffsets[gr];
        uint CellHash = GetCellHash(lookupCell);
        uint CellIndex = CellIndexFromHash(CellHash);
        for ( uint lookupIndex = StartIndices[CellIndex]; lookupIndex < ParticleCount; ++lookupIndex )
        {
            if ( SortedParticles[lookupIndex].CellIndex != CellIndex )
                break;
            uint otherP = SortedParticles[lookupIndex].pIndex;

            // now we got all the indices
            vec2 otherPosition = Particles[otherP].Position;
            vec2 offset = otherPosition - position;
            float dst2 = dot(offset, offset);
            if ( dst2 > SmoothingRadius*SmoothingRadius ) continue;
            
            // now we have all the data
            float dst = sqrt(dst2);
            Density += ParticleMass * DensityKernel(dst);
        }
    }
    return Density;
}

float DensityKernel(float dst)
{
    float v = SmoothingRadius - dst;
	return v * v * DensityScale;
}

// GRID

ivec2 GetCellCoord(vec2 Position)
{
	vec2 normilizedPos = Position + WorldSize;
	int cellX = int(floor(normilizedPos.x / SmoothingRadius*2.0f));
    int cellY = int(floor(normilizedPos.y / SmoothingRadius*2.0f));
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
    return Hash % ParticleCount;
}