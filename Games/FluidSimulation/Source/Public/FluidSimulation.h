#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "IBravoRenderable.h"

struct Particle
{
	alignas(8) glm::vec2 Position = glm::vec2(0.0f);
	alignas(8) glm::vec2 Velocity = glm::vec2(0.0f);
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
	 
	int32 ParticleCount = 500;
	bool bRandomPositions = false;

	float ParticleMass = 1.0f;

	float ParticleSize = 1.0f; 

	float CollisionDamping = 0.3f;

	float Gravity = 0.0f;

	float MaxVelocity = 100.0f;

	float SmoothingRadius = 5.0f;

	float TargetDensity = 1.0f;

	float Preassure = 50.0f;

	// END SIMULATION PROPERTIES

	void SpawnParticles(int32 Count, bool bRandomPos);
	
	void Reset();
	void TogglePause();
	bool IsPaused() const { return bPaused; }
	bool HasStarted() const { return bHasStarted; }

	GLuint GetParticlesSSBO() const { return ParticlesSSBO; }

private:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;


	void SimulationStep(float DeltaTime);

	glm::vec2 CalcPressureForce(int32 i) const;
	float CalcDensity(const glm::vec2& samplePoint) const;
	float SmoothingKernel(float radius, float distance) const;
	float SmoothingKernelDerivative(float radius, float distance) const;

	float CalcSharedPressure(float densityA, float densityB) const;
	float DensityToPeassure(float density) const;

private:
	std::vector<Particle> Particles;
	std::shared_ptr<class FluidContainer> ParentContainer = nullptr;

	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint ParticlesSSBO = 0;
	int32 ParticlesCount = 0;

	std::shared_ptr<class BravoShaderAsset> Shader;


	bool bHasStarted = false;
	bool bPaused = true;
	std::vector<glm::vec2> OriginalPositions;

	std::vector<int32> ParticleIndicies;
	std::vector<float> ParticleDensities;
};
