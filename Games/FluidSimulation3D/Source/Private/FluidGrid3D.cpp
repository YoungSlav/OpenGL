#include "FluidGrid3D.h"

void FluidGrid3D::UpdateSize(const glm::vec2& _WorldSize, float Radius)
{
	WorldSize = _WorldSize;
	Size = Radius * 2.0f;
	GridSize.x = glm::ceil(WorldSize.x / Size);
    GridSize.y = glm::ceil(WorldSize.y / Size);
}

void FluidGrid3D::UpdateGrid(const std::vector<int32>& ParticleIndicies, const std::vector<glm::vec2>& Positions)
{
	ParticleCount = ParticleIndicies.size();
	if ( !ParticleCount )
		return;

	Lookup.resize(ParticleCount, SpacialLookup());
	StartIndices.resize(ParticleCount, Lookup.size());

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, Positions](int32 i)
		{
			CellHash cHash;
			glm::ivec2 CellCoords;
			GetCellCoord(Positions[i], CellCoords);
			GetCellHash(CellCoords, cHash);
			Lookup[i].cHash = cHash;
			Lookup[i].pIndex = i;
			StartIndices[i] = StartIndices.size();
		});


	std::sort(
		Lookup.begin(),
		Lookup.end(),
		[this](const SpacialLookup& left, const SpacialLookup& right)
		{	
			return left.cHash.CellIndex < right.cHash.CellIndex;
		});

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this](int32 lookupIndex)
		{
			uint32 CellIndex = Lookup[lookupIndex].cHash.CellIndex;
			uint32 prevCellIndex = lookupIndex != 0 ? Lookup[lookupIndex-1].cHash.CellIndex : -1;
			if ( CellIndex != prevCellIndex )
			{
				StartIndices[CellIndex] = lookupIndex;
			}
		});
}

void FluidGrid3D::GetRelatedParticles(const glm::vec2& Position, std::list<int32>& OutParticles) const
{
	OutParticles.clear();

	if ( !ParticleCount )
		return;

	std::list<glm::ivec2> offsets {
		glm::ivec2(0, 0),
		glm::ivec2(1, 0),
		glm::ivec2(-1, 0),
		glm::ivec2(0, 1),
		glm::ivec2(0, -1),
		glm::ivec2(1, 1),
		glm::ivec2(1, -1),
		glm::ivec2(-1, 1),
		glm::ivec2(-1, -1)
	};
	
	glm::ivec2 CellCoord;
	GetCellCoord(Position, CellCoord);

	//assert(CellCoord.x >= 0 && CellCoord.y >= 0 && CellCoord.x < GridSize.x && CellCoord.y < GridSize.y);
	for ( auto it : offsets )
		GetParticlesInCell(CellCoord+it, OutParticles);
}

void FluidGrid3D::GetParticlesInCell(const glm::ivec2& CellCoords, std::list<int32>& OutParticles) const
{
	CellHash cHash;
	GetCellHash(CellCoords, cHash);

	if ( StartIndices.size() <= cHash.CellIndex || cHash.CellIndex < 0 )
		return;

	int32 StartIndex = StartIndices[cHash.CellIndex];
		
	for (  uint32 i = StartIndex; i < Lookup.size(); ++i)
	{
		if ( Lookup[i].cHash.CellIndex != cHash.CellIndex )
			return;
		if ( Lookup[i].cHash.Hash != cHash.Hash )
			continue;

		OutParticles.push_back(Lookup[i].pIndex);
	}
}

void FluidGrid3D::GetCellCoord(const glm::vec2& Position, glm::ivec2& OutCoords) const
{
	glm::vec2 normilizedPos = Position + WorldSize * 0.5f;
	int32 cellX = static_cast<int32>(std::floor(normilizedPos.x / Size));
    int32 cellY = static_cast<int32>(std::floor(normilizedPos.y / Size));
	OutCoords = glm::ivec2(cellX, cellY);
}

void FluidGrid3D::GetCellHash(const glm::ivec2& Coords, CellHash& OutHash) const
{	
	const uint32 PRIME1 = 0x9e3779b1;
    const uint32 PRIME2 = 0x85ebca6b;

	OutHash.Hash = Coords.y * PRIME1 + Coords.x * PRIME2;;
	OutHash.CellIndex = OutHash.Hash % ParticleCount;
}