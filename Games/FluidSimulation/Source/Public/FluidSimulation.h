#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "IBravoRenderable.h"

struct Particle
{
	alignas(8) glm::vec2 Position = glm::vec2(0.0f);
	alignas(8) glm::vec2 Velocity = glm::vec2(0.0f);
	int32 Hightligh = 0;
};

struct CellHash
{
	// index of a cell normalized to particle count
	int32 CellIndexP = -1;
	// index of a cell
	int32 CellIndex = -1;
};

struct SpacialLookup
{
	CellHash cHash;
	int32 pIndex = -1;
};


class FluidSimulation : public BravoObject, public IBravoTickable, public IBravoRenderable
{
public:
	template <typename... Args>
	FluidSimulation(std::shared_ptr<class FluidContainer> _ParentContainer, Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTickable(),
		IBravoRenderable(ERenderPriority::Early),
		ParentContainer(_ParentContainer)
	{}

	// SIMULATION PROPERTIES
	 
	int32 ParticlesCount = 20;
	bool bRandomPositions = false;

	float ParticleMass = 1.0f;
	float ParticleSize = 0.1f; 



	float CollisionDamping = 0.3f;
	float Gravity = 100.0f;

	float SmoothingRadius = 0.5f;
	float TargetDensity = 0.25f;
	float Preassure = 0.7f;



	float MaxVelocity = 100.0f;
	int32 StepsPerTick = 1;

	// END SIMULATION PROPERTIES

	void SpawnParticles();
	
	void Reset();
	void TogglePause();
	bool IsPaused() const { return bPaused; }
	bool HasStarted() const { return bHasStarted; }

	GLuint GetParticlesSSBO() const { return ParticlesSSBO; }

private:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;

	void OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime);


	void SimulationStep(float DeltaTime);

	glm::vec2 CalcPressureForce(int32 i) const;
	float CalcDensity(const glm::vec2& samplePoint) const;
	float SmoothingKernel(float radius, float distance) const;
	float SmoothingKernelDerivative(float radius, float distance) const;

	float CalcSharedPressure(float densityA, float densityB) const;
	float DensityToPeassure(float density) const;

	void GetRelatedParticles(const glm::vec2& Position, std::list<int32>& OutParticles) const;
	void GetParticlesInCell(const glm::ivec2& CellIndex, std::list<int32>& OutParticles) const;
	void UpdateSpacialLookup();
	glm::ivec2 GetCellCoord(const glm::vec2& Position) const;
	bool GetCellHash(const glm::ivec2& Coords, CellHash& OutHash) const;

private:
	std::vector<Particle> Particles;
	std::shared_ptr<class FluidContainer> ParentContainer = nullptr;

	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint ParticlesSSBO = 0;
	int32 CachedCount = 0;

	std::shared_ptr<class BravoShaderAsset> Shader;


	glm::vec2 WorldSize = glm::vec2(0.0f);

	bool bHasStarted = false;
	bool bPaused = true;
	std::vector<glm::vec2> OriginalPositions;

	std::vector<int32> ParticleIndicies;
	std::vector<float> ParticleDensities;

	


	// cell hash <-> particle index
	std::vector<SpacialLookup> Lookup;
	// index = cell hash, value = start index in SpacialLookup
	std::vector<int32> StartIndices;
	// index = cell hash, value - occupation
	std::vector<bool> CellOccupied;
	
	glm::ivec2 SmoothingGridSize;
	glm::ivec2 ParticleGridSize;
};
