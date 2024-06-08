#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "IBravoRenderable.h"
#include "FluidMath.h"
#include "FluidGrid.h"

struct Particle
{
	alignas(8) glm::vec2 Position = glm::vec2(0.0f);
    alignas(8) glm::vec2 PredictedPosition = glm::vec2(0.0f);
    alignas(8) glm::vec2 Velocity = glm::vec2(0.0f);
    int Highlight;
    float Density;
    float NearDensity;
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
	bool bRandomPositions = false;

	float ParticleMass = 1.0f;
	float ParticleRadius = 0.1f; 
	float SmoothingRadius = 0.2f;

	float TargetDensity = 236.0f;
	float Preassure = 0.0f;
	float NearPressureMultiplier = 230.0f;
	float ViscosityFactor = 0.18f;


	glm::vec3 Cold = glm::vec3(5.0f, 49.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Middle = glm::vec3(5.0f, 106.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Hot = glm::vec3(192.0f, 233.0f, 248.0f) / glm::vec3(255.0f);

	float InteractionAcceleration = 20.0f;
	float InteractionRadius = 50.0f;
	float CollisionDamping = 0.3f;
	float Gravity = 10.0f;

	float MaxVelocity = 15.0;

	uint32 StepsPerTick = 8;

	// END SIMULATION PROPERTIES

	void SpawnParticles();
	
	void Reset();
	void TogglePause();
	bool IsPaused() const { return bPaused; }

	bool HasStarted() const { return bHasStarted; }
	void UpdateShaderUniformParams();
	GLuint GetParticlesSSBO() const { return ParticlesSSBO; }

private:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;
	void SimulationStep(float DeltaTime);
	virtual void Render() override;

	void OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime);
	void OnInput_MOUSERIGHT(bool ButtonState, float DeltaTime);
	void OnInput_MOUSELEFT(bool ButtonState, float DeltaTime);
	void OnInput_Space(bool ButtonState, float DeltaTime);
	void OnInput_R(bool ButtonState, float DeltaTime);

	void SetMouseForce(const glm::vec2& MouseLocation, float Dir);

	
	void FillBuffers();

private:
	std::vector<Particle> Particles;
	std::shared_ptr<class FluidContainer> ParentContainer = nullptr;

	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint ParticlesSSBO = 0;
	GLuint SortedParticlesSSBO = 0;
	GLuint StartIndicesSSBO = 0;
	int32 CachedParticlesCount = 0;
	GLuint NumWorkGroups;

	std::shared_ptr<class BravoShaderAsset> RenderShader;

	std::shared_ptr<class BravoShaderAsset> ExternalForcesCompute;
	std::shared_ptr<class BravoShaderAsset> GridHashingCompute;
	std::shared_ptr<class BravoShaderAsset> GridSortingCompute;
	std::shared_ptr<class BravoShaderAsset> PressureCompute;


	glm::vec2 WorldSize = glm::vec2(0.0f);

	bool bHasStarted = false;
	bool bPaused = true;
	std::vector<glm::vec2> OriginalPositions;


	bool bMouseLeft = false;
	bool bMouseRight = false;
	glm::vec2 InteractionLocation;
	float InteractionForce = 0.0f;

	int32 CurrentSimulationStep = 0;
	int32 SimulationTarget = 0;
};
