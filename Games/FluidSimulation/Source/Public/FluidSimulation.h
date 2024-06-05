#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "IBravoRenderable.h"
#include "FluidMath.h"
#include "FluidGrid.h"

struct Particle
{
	alignas(8) glm::vec2 Position = glm::vec2(0.0f);
	alignas(8) glm::vec2 Velocity = glm::vec2(0.0f);
	int32 Hightligh = 0;
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
	 
	int32 ParticlesCount = 8000;
	bool bRandomPositions = true;

	float ParticleMass = 50.0f;
	float ParticleSize = 5.0f; 
	float SmoothingRadius = 15.0f;



	float CollisionDamping = 0.3f;
	float Gravity = 200.0f;

	float TargetDensity = 0.5f;
	float Preassure = 300.0f;
	float NearPressureMultiplier = 1500.0f;

	glm::vec3 Cold = glm::vec3(5.0f, 49.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Middle = glm::vec3(5.0f, 106.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Hot = glm::vec3(192.0f, 233.0f, 248.0f) / glm::vec3(255.0f);


	glm::vec2 InteractionLocation;
	float InteractionForce = 0.0f;

	float ViscosityFactor = 3000.0f;

	float InteractionAcceleration = 1500.0f;
	float InteractionStep = 10.0f;
	float InteractionRadius = 100000.0f;
	float MaxVelocity = 300.0f;
	int32 StepsPerTick = 1;

	// END SIMULATION PROPERTIES

	void SpawnParticles();
	
	void Reset();
	void TogglePause();
	bool IsPaused() const { return bPaused; }
	void UpdateMath()
	{
		math.SetRadius(SmoothingRadius);
	}

	bool HasStarted() const { return bHasStarted; }
	GLuint GetParticlesSSBO() const { return ParticlesSSBO; }


private:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;

	void OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime);
	void OnMouseScroll(const glm::vec2& DeltaScroll, float DeltaTime);
	void OnInput_MOUSERIGHT(bool ButtonState, float DeltaTime);
	void OnInput_MOUSELEFT(bool ButtonState, float DeltaTime);
	void OnInput_Space(bool ButtonState, float DeltaTime);


	void SimulationStep(float DeltaTime);

	void CalcDensity(const glm::vec2& samplePoint, float& Density, float& NearDensity) const;
	glm::vec2 CalcExternalForces(int32 i) const;
	glm::vec2 CalcPressureForce(int32 i) const;
	glm::vec2 CalcViscosity(int32 i) const;

	float DensityToPessure(float density) const;
	float NearDensityToPessure(float density) const;

	float DensityKernel(float dst) const;
	float NearDensityKernel(float dst) const;
	float DensityDerivative(float dst) const;
	float NearDensityDerivative(float dst) const;
	float ViscosityKernel(float dst) const;

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
	std::vector<glm::vec2> PredictedPositions;
	std::vector<float> Densities;
	std::vector<float> NearDensities;

	FluidMath math;
	FluidGrid Grid;

	bool bMouseLeft = false;
	bool bMouseRight = false;

	glm::ivec2 SmoothingGridSize;
	glm::ivec2 ParticleGridSize;
};
