#include "FluidGrid.h"

void FluidGrid::UpdateSize(const glm::vec2& _WorldSize, float _ParticleRadius)
{
	WorldSize = _WorldSize;
	ParticleRadius = _ParticleRadius;
	GridSize.x = glm::ceil(WorldSize.x / ParticleRadius * 2.0f);
    GridSize.y = glm::ceil(WorldSize.y / ParticleRadius * 2.0f);
}

void FluidGrid::UpdateGrid(const std::vector<int32>& ParticleIndicies, const std::vector<glm::vec2>& Positions)
{
	ParticleCount = ParticleIndicies.size();

	Lookup.resize(ParticleCount, SpacialLookup());
	StartIndices.resize(ParticleCount, -1);
	CellOccupied.resize( GridSize.x * GridSize.y, false);

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, Positions](int32 i)
		{
			CellHash cHash;
			glm::ivec2 CellCoords;
			GetCellCoord(Positions[i], CellCoords);
			if ( !GetCellHash(CellCoords, cHash) )
				return;
			Lookup[i].cHash = cHash;
			Lookup[i].pIndex = i;
		});

	for ( const SpacialLookup& l : Lookup )
	{
		if ( l.cHash.CellIndex < 0 ) continue;
		CellOccupied[l.cHash.CellIndex] = true;
	}

	std::sort(
		Lookup.begin(),
		Lookup.end(),
		[this](const SpacialLookup& left, const SpacialLookup& right)
		{	
			return left.cHash.CellIndexP < right.cHash.CellIndexP;
		});

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this](int32 lookupIndex)
		{
			int32 CellHash = Lookup[lookupIndex].cHash.CellIndexP;
			int32 prevCellHash = lookupIndex != 0 ? Lookup[lookupIndex-1].cHash.CellIndexP : -1;
			if ( CellHash != prevCellHash )
			{
				StartIndices[CellHash] = lookupIndex;
			}
		});
}

void FluidGrid::GetRelatedParticles(const glm::vec2& Position, std::list<int32>& OutParticles) const
{
	OutParticles.clear();

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
	for ( auto it : offsets )
		GetParticlesInCell(CellCoord+it, OutParticles);
}

void FluidGrid::GetParticlesInCell(const glm::ivec2& CellCoords, std::list<int32>& OutParticles) const
{
	CellHash cHash;
	if ( !GetCellHash(CellCoords, cHash) )
		return;

	if ( !CellOccupied[cHash.CellIndex] )
		return;

	int32 CellKey = cHash.CellIndexP;

	if ( StartIndices.size() <= CellKey || CellKey < 0 )
		return;

	int32 StartIndex = StartIndices[CellKey];

	if ( StartIndex < 0 )
		return;
	
	for (  uint32 i = StartIndex; i < Lookup.size(); ++i)
	{
		if ( Lookup[i].cHash.CellIndexP != CellKey )
			return;

		OutParticles.push_back(Lookup[i].pIndex);
	}
}

void FluidGrid::GetCellCoord(const glm::vec2& Position, glm::ivec2& OutCoords) const
{
	glm::vec2 normilizedPos = Position + WorldSize * 0.5f;
	int32 cellX = static_cast<int32>(std::floor(normilizedPos.x / ParticleRadius*0.5f));
    int32 cellY = static_cast<int32>(std::floor(normilizedPos.y / ParticleRadius*0.5f));
	OutCoords = glm::ivec2(cellX, cellY);
}

bool FluidGrid::GetCellHash(const glm::ivec2& Coords, CellHash& OutHash) const
{
	if (Coords.x < 0 || Coords.y < 0 || Coords.x >= GridSize.x || Coords.y >= GridSize.y)
	{
		return false;
	}
	int32 hash = Coords.y * GridSize.x + Coords.x;
	int32 cellIndex = hash % CellOccupied.size();
	int32 pIndex = hash % ParticleCount;

	OutHash.CellIndex = cellIndex;
	OutHash.CellIndexP = pIndex;
	return true;
}