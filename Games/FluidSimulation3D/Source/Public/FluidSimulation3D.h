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
};

struct Density
{
	alignas(4) float Density;
	alignas(4) float iDensity;
	alignas(4) float NearDensity;
	alignas(4) float iNearDensity;
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

	glm::vec3 SimulationBounds = glm::vec3(15.0f, 15.0f, 5.0f);

	float ParticleMass = 1.0f;
	int32 ParticleCount = 50000;
	
	float ParticleRadius = 0.1f; 
	float SmoothingRadius = ParticleRadius * 4.0f;

	float TargetDensity = 150.0f;
	float PressureFactor = 300.0f;
	float NearPressureFactor = TargetDensity * 0.01f;
	float ViscosityFactor = 0.05f;

	float CollisionDamping = 0.05f;
	float Gravity = 10.0f;

	uint32 StepsPerTick = 2;

	float MaxVelocity = 6.0;

	glm::vec3 Cold = glm::vec3(5.0f, 49.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Middle = glm::vec3(5.0f, 106.0f, 111.0f) / glm::vec3(255.0f);
	glm::vec3 Hot = glm::vec3(192.0f, 233.0f, 248.0f) / glm::vec3(255.0f);

	
	float DensityScale;
	float NearDensityScale;
	float PressureScale;
	float NearPressureScale;
	float ViscosityScale;
	

	

	// END SIMULATION PROPERTIES
		
	void Reset();
	void TogglePause();
	bool IsPaused() const { return bPaused; }
	bool HasStarted() const { return bStarted; }

	std::shared_ptr<class BravoBoundingBox> GetBoundingBox() const { return BoundingBox; }

	void UpdateShaderUniformParams();

protected:
	virtual bool Initialize_Internal() override;

	void OnBoundingBoxTransofrmUpdated(const class IBravoTransformable*);

	

	virtual void Tick(float DeltaTime) override;
	void SimulationStep(float DeltaTime);
	
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
	GLuint DensitiesSSBO = 0;
	
	GLuint SortedCellIndiciesSSBO = 0;
	GLuint RadixTmpSSBO = 0;
	GLuint RadixHistogramSSBO = 0;

	GLuint StartIndicesSSBO = 0;
	GLuint NumWorkGroups;

	float SimulationStartTime = 0.0f;
	bool bStarted = false;

	

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
	std::shared_ptr<class BravoShaderAsset> DensityCompute;
	std::shared_ptr<class BravoShaderAsset> PressureCompute;


	bool bPaused = true;
	bool bReadyToRender = false;

	std::shared_ptr<class BravoBoundingBox> BoundingBox;

	

	// particle
	GLuint ParticleVAO;
	GLuint ParticleVBO = 0;
};
