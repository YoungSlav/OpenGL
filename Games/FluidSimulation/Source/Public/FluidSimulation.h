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
    float Density;
	float iDensity;
	float NearDensity;
	float iNearDensity;
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

	int32 ParticleCount = 84000;
	bool bRandomPositions = false;

	float ParticleMass = 0.1f;
	float ParticleRadius = 0.021f; 

	float TargetDensity = 20.0f;
	float Preassure = 10.0f;
	float ViscosityFactor = 0.03f;

	float SmoothingRadius = ParticleRadius * 6.0f;
	float NearPreassure = TargetDensity * 0.0001f;

	float InteractionAcceleration = 50.0f;
	float InteractionRadius = 2.0f;
	float CollisionDamping = 0.1f;
	float Gravity = 9.8f;


	glm::vec3 Cold = glm::vec3(5.0f, 49.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Middle = glm::vec3(5.0f, 106.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Hot = glm::vec3(192.0f, 233.0f, 248.0f) / glm::vec3(255.0f);
	float MaxVelocity = 10.0;

	uint32 StepsPerTick = 4;

	glm::vec2 WorldSize;

	// END SIMULATION PROPERTIES
		
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
	
	void ExecuteRadixSort();
	void UpdateRadixIteration(int32 i);

	virtual void Render() override;

	void OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime);
	void OnInput_MOUSERIGHT(bool ButtonState, float DeltaTime);
	void OnInput_MOUSELEFT(bool ButtonState, float DeltaTime);
	void OnInput_Space(bool ButtonState, float DeltaTime);
	void OnInput_R(bool ButtonState, float DeltaTime);

	void SetMouseForce(const glm::vec2& MouseLocation, float Dir);

	
	void FillBuffers();

private:
	std::shared_ptr<class FluidContainer> ParentContainer = nullptr;

	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint ParticlesSSBO = 0;
	
	GLuint SortedParticlesSSBO = 0;
	GLuint RadixTmpSSBO = 0;
	GLuint RadixHistogramSSBO = 0;

	GLuint StartIndicesSSBO = 0;
	GLuint NumWorkGroups;

	std::shared_ptr<class BravoShaderAsset> RenderShader;

	std::shared_ptr<class BravoShaderAsset> SpawnParticlesCompute;

	std::shared_ptr<class BravoShaderAsset> GridHashingCompute;
	
	struct RadixSortConstants
	{
		uint32 g_num_elements;
		uint32 g_shift;
		uint32 g_num_workgroups;
		uint32 g_num_blocks_per_workgroup;
	} RadixSortPushConstants;
	GLuint RadixSortConstantsUBO;

	std::shared_ptr<class BravoShaderAsset> RadixSortCompute;
	std::shared_ptr<class BravoShaderAsset> RadixSortHistogramCompute;

	std::shared_ptr<class BravoShaderAsset> FluidStartingIndiciesCompute;
	std::shared_ptr<class BravoShaderAsset> PressureCompute;


	

	bool bHasStarted = false;
	bool bPaused = true;


	bool bMouseLeft = false;
	bool bMouseRight = false;
	glm::vec2 InteractionLocation;
	float InteractionForce = 0.0f;
};
