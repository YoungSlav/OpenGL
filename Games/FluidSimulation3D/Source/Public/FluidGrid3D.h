#pragma once
#include "stdafx.h"

struct CellHash
{
	// index of a cell normalized to particle count
	uint32 Hash = 0;
	// index of a cell
	uint32 CellIndex = 1;
};

struct SpacialLookup
{
	CellHash cHash;
	int32 pIndex = -1;
};

class FluidGrid3D
{
public:
	void UpdateSize(const glm::vec2& _WorldSize, float _ParticleRadius);
	void UpdateGrid(const std::vector<int32>& ParticleIndicies, const std::vector<glm::vec2>& Positions);
	
	void GetRelatedParticles(const glm::vec2& Position, std::list<int32>& OutParticles) const;

private:
	void GetParticlesInCell(const glm::ivec2& CellIndex, std::list<int32>& OutParticles) const;
	void GetCellCoord(const glm::vec2& Position, glm::ivec2& OutCoords) const;
	void GetCellHash(const glm::ivec2& Coords, CellHash& OutHash) const;

private:
	int32 ParticleCount;
	// cell hash <-> particle index
	std::vector<SpacialLookup> Lookup;
	// index = cell hash, value = start index in SpacialLookup
	std::vector<int32> StartIndices;

	glm::vec2 WorldSize;
	float Size;
	glm::ivec2 GridSize;
};
