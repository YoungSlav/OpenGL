#pragma once
#include "stdafx.h"
#include "BravoActor.h"
#include "IBravoRenderable.h"
#include "FluidMath3D.h"
#include "FluidGrid3D.h"

struct Particle
{
	alignas(16) glm::vec3 Position = glm::vec3(0.0f);
    alignas(16) glm::vec3 PredictedPosition = glm::vec3(0.0f);
    alignas(16) glm::vec3 Velocity = glm::vec3(0.0f);
    alignas(4) float Density;
	alignas(4) float iDensity;
};

class FluidSimulation3D : public BravoActor, public IBravoTickable, public IBravoRenderable
{
public:
	template <typename... Args>
	FluidSimulation3D(Args&&... args) :
		BravoActor(std::forward<Args>(args)...),
		IBravoTickable(),
		IBravoRenderable()
	{}

	// SIMULATION PROPERTIES

	const float scale = 2.0f;
	int32 ParticleCount = 100000;
	
	float ParticleMass = 1.0f;
	float ParticleRadius = 0.5f; 
	float SmoothingRadius = 2.0f;

	float TargetDensity = 1.2f;
	float PressureFactor = 240.0f;
	float ViscosityFactor = 5.0f;


	glm::vec3 Cold = glm::vec3(5.0f, 49.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Middle = glm::vec3(5.0f, 106.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Hot = glm::vec3(192.0f, 233.0f, 248.0f) / glm::vec3(255.0f);

	float CollisionDamping = 0.1f;
	float Gravity = 20.0f;

	float MaxVelocity = 20.0;

	uint32 StepsPerTick = 4;

	// END SIMULATION PROPERTIES
		
	void Reset();
	void TogglePause();
	bool IsPaused() const { return bPaused; }

protected:
	virtual bool Initialize_Internal() override;

	void OnBoundingBoxTransofrmUpdated(const class IBravoTransformable*);

	void UpdateShaderUniformParams();

	virtual void Tick(float DeltaTime) override;
	void SimulationStep(float DeltaTime);
	
	void PrepareGrid(float DeltaTime);
	void ExecuteRadixSort();
	void UpdateRadixIteration(int32 i);

	virtual void Render() override;

	
	void OnInput_Space(bool ButtonState, float DeltaTime);
	void OnInput_R(bool ButtonState, float DeltaTime);

	
	void FillBuffers();

private:
	BravoTransform ContainerTransform;

	GLuint ParticlesSSBO = 0;
	
	GLuint SortedParticlesSSBO = 0;
	GLuint RadixTmpSSBO = 0;
	GLuint RadixHistogramSSBO = 0;

	GLuint StartIndicesSSBO = 0;
	GLuint NumWorkGroups;

	std::shared_ptr<class BravoShaderAsset> RenderShader;

	std::shared_ptr<class BravoShaderAsset> ParticleGenerationCompute;
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


	bool bPaused = true;
	bool bReadyToRender = false;

	std::shared_ptr<class BravoBoundingBox> BoundingBox;

	std::shared_ptr<class FluidPostProcess> RaymarchingPP = nullptr;

	// particle
	GLuint ParticleVAO;
	GLuint ParticleVBO = 0;
};
