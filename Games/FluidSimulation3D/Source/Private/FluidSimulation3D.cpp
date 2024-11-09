#include "FluidSimulation3D.h"
#include "BravoBoundingBox.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoCamera.h"
#include "BravoInput.h"
#include "BravoViewport.h"
#include "BravoStaticMeshAsset.h"

bool FluidSimulation3D::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;
	

	glGenVertexArrays(1, &ParticleVAO);
	glBindVertexArray(ParticleVAO);
	glGenBuffers(1, &ParticlesSSBO);
	glBindBuffer(GL_ARRAY_BUFFER, ParticlesSSBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, Position));
    // PredictedPosition
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, PredictedPosition));
    // Velocity
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, Velocity));
    // Density
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, Density));
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
		
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	glGenBuffers(1, &SortedParticlesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SortedParticlesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &RadixTmpSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RadixTmpSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &RadixHistogramSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RadixHistogramSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	glGenBuffers(1, &StartIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, StartIndicesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &RadixSortConstantsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, RadixSortConstantsUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(RadixSortConstantsUBO), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto AssetManager = Engine->GetAssetManager();

	BoundingBox = NewObject<BravoBoundingBox>("SimulationBoundingBox");
	BoundingBox->SetScale(glm::vec3(20.0f, 20.0f, 10.0f));
	BoundingBox->OnTransformUpdated.AddSP(Self<FluidSimulation3D>(), &FluidSimulation3D::OnBoundingBoxTransofrmUpdated);
	
	RenderShader = AssetManager->FindOrLoad<BravoShaderAsset>("FluidParticleShader", BravoShaderLoadingParams("Shaders\\FluidParticle3D"));

	ParticleGenerationCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidGenerateParticles", BravoShaderLoadingParams("Shaders\\Compute\\FluidGenerateParticles"));

	ExternalForcesCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidExternalForces", BravoShaderLoadingParams("Shaders\\Compute\\FluidExternalForces"));
	GridHashingCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidGridHashing", BravoShaderLoadingParams("Shaders\\Compute\\FluidGridHashing"));
	
	RadixSortCompute = AssetManager->FindOrLoad<BravoShaderAsset>("RadixSort", BravoShaderLoadingParams("Shaders\\Compute\\ThirdParty\\multi_radixsort"));
	RadixSortHistogramCompute = AssetManager->FindOrLoad<BravoShaderAsset>("RadixSortHistogram", BravoShaderLoadingParams("Shaders\\Compute\\ThirdParty\\multi_radixsort_histograms"));

	FluidStartingIndiciesCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidStartingIndicies", BravoShaderLoadingParams("Shaders\\Compute\\FluidStartingIndicies"));
	PressureCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidPressure", BravoShaderLoadingParams("Shaders\\Compute\\FluidPressure"));

	{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_SPACE;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<FluidSimulation3D>(), &FluidSimulation3D::OnInput_Space);
		Engine->GetInput()->SubscribeKey(subscription);
	}
	{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_R;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<FluidSimulation3D>(), &FluidSimulation3D::OnInput_R);
		Engine->GetInput()->SubscribeKey(subscription);
	}

	Reset();

	return true;
}

void FluidSimulation3D::FillBuffers()
{
	NumWorkGroups = (ParticlesCount + 1023) / 1024;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ParticlesCount * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SortedParticlesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ParticlesCount * sizeof(uint32) * 3, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SortedParticlesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RadixTmpSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ParticlesCount * sizeof(uint32) * 3, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, RadixTmpSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RadixHistogramSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NumWorkGroups * 256 * sizeof(uint32), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, RadixHistogramSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, StartIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ParticlesCount * sizeof(uint32), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, StartIndicesSSBO);

	glBindBuffer(GL_UNIFORM_BUFFER, RadixSortConstantsUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, RadixSortConstantsUBO);

	RadixSortPushConstants.g_num_elements = ParticlesCount;
	RadixSortPushConstants.g_num_workgroups = NumWorkGroups;
	RadixSortPushConstants.g_num_blocks_per_workgroup = 1024;
}

void FluidSimulation3D::OnBoundingBoxTransofrmUpdated(const class IBravoTransformable*)
{
	glm::vec3 ContainerSize = BoundingBox->GetScale();

	GridHashingCompute->Use();
		GridHashingCompute->SetFloat2("WorldSize", ContainerSize);
	GridHashingCompute->StopUsage();

	PressureCompute->Use();
		PressureCompute->SetFloat2("WorldSize", ContainerSize);
		PressureCompute->SetMatrix4d("BoundingBoxModel",
			BoundingBox->GetTransform().GetTransformMatrix());
	PressureCompute->StopUsage();
}

void FluidSimulation3D::UpdateShaderUniformParams()
{
	glm::vec3 ContainerSize = BoundingBox->GetScale();
	// external force
	ExternalForcesCompute->Use();
		ExternalForcesCompute->SetFloat1("GravityForce", Gravity);
	ExternalForcesCompute->StopUsage();

	// hashing
	GridHashingCompute->Use();
		GridHashingCompute->SetFloat2("WorldSize", ContainerSize);
		GridHashingCompute->SetFloat1("SmoothingRadius", SmoothingRadius);
	GridHashingCompute->StopUsage();
	
	// pressure
	PressureCompute->Use();
		PressureCompute->SetFloat2("WorldSize", ContainerSize);
		PressureCompute->SetMatrix4d("BoundingBoxModel",
			BoundingBox->GetTransform().GetTransformMatrix());
		PressureCompute->SetFloat1("MaxVelocity", MaxVelocity);

		PressureCompute->SetFloat1("ParticleMass", ParticleMass);
		PressureCompute->SetFloat1("ParticleRadius", ParticleRadius);
		PressureCompute->SetFloat1("SmoothingRadius", SmoothingRadius);
		PressureCompute->SetFloat1("TargetDensity", TargetDensity);
	
		PressureCompute->SetFloat1("PressureFactor", PressureFactor);
		PressureCompute->SetFloat1("ViscosityFactor", ViscosityFactor);
		PressureCompute->SetFloat1("CollisionDamping", CollisionDamping);

		const float DensityScale	= 315.0f / (64.0f * (glm::pi<float>() * glm::pow(SmoothingRadius, 9)));
		const float PressureScale	= -45.0f / (glm::pi<float>() * glm::pow(SmoothingRadius, 6));
		const float ViscosityScale	= 45.0f  / (glm::pi<float>() * glm::pow(SmoothingRadius, 5));
		
		PressureCompute->SetFloat1("DensityScale", DensityScale);
		PressureCompute->SetFloat1("PressureScale", PressureScale);
		PressureCompute->SetFloat1("ViscosityScale", ViscosityScale);
	PressureCompute->StopUsage();
}

void FluidSimulation3D::Render()
{
	if ( ParticlesCount <= 0 && bReadyToRender)
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;
	
	const glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	const glm::mat4 CameraView = camera->GetViewMatrix();
	const glm::mat4 model = GetTransform_World().GetTransformMatrix();

	const glm::mat4 ViewProj = CameraProjection * CameraView;
	
	RenderShader->Use();

		RenderShader->SetMatrix4d("model", model);

		RenderShader->SetMatrix4d("viewProj", ViewProj);
		RenderShader->SetFloat1("particleSize", ParticleRadius);
		RenderShader->SetFloat1("maxSpeed", MaxVelocity);

		RenderShader->SetFloat3("Cold", Cold);
		RenderShader->SetFloat3("Middle", Middle);
		RenderShader->SetFloat3("Hot", Hot);
				
		glBindVertexArray(ParticleVAO);
			glDrawArrays(GL_POINTS, 0, ParticlesCount);
		glBindVertexArray(0);

	RenderShader->StopUsage();
}

void FluidSimulation3D::Reset()
{
	FillBuffers();
	UpdateShaderUniformParams();
	
	bPaused = true;

	ParticleGenerationCompute->Use();
		// update time step
		ParticleGenerationCompute->SetInt("ParticleCount", ParticlesCount);
		ParticleGenerationCompute->SetMatrix4d("BoundingBox", BoundingBox->GetTransform().GetTransformMatrix());
		
		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	ParticleGenerationCompute->StopUsage();

	bReadyToRender = true;
}

void FluidSimulation3D::TogglePause()
{
	bPaused = !bPaused;
}

void FluidSimulation3D::OnInput_Space(bool ButtonState, float DeltaTime)
{
	TogglePause();	
}
void FluidSimulation3D::OnInput_R(bool ButtonState, float DeltaTime)
{
	Reset();
}

///////////////////////////////////////////////

void FluidSimulation3D::Tick(float DeltaTime)
{
	if ( bPaused ) return;

	//BoundingBox->SetRotation(glm::vec3(0.0f, 0.0f, sin(LifeTime)*20.0f));
	BoundingBox->SetScale(glm::vec3(40 + cos(LifeTime)*10.0f, 40.0f, 10.0f));

	for ( int32 i = 0; i < StepsPerTick; ++i )
		SimulationStep(DeltaTime / StepsPerTick);
}

void FluidSimulation3D::SimulationStep(float DeltaTime)
{
	if ( bPaused ) return;

	ExternalForcesCompute->Use();
		// update time step
		ExternalForcesCompute->SetFloat1("SimulationTimeStep", 1.0f / (120.0f * StepsPerTick));
		
		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	ExternalForcesCompute->StopUsage();
	
	GridHashingCompute->Use();
		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	GridHashingCompute->StopUsage();


	ExecuteRadixSort();
	
	FluidStartingIndiciesCompute->Use();
		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	FluidStartingIndiciesCompute->StopUsage();

	PressureCompute->Use();
		// update time step
		PressureCompute->SetFloat1("SimulationTimeStep", DeltaTime);

		glm::vec2 RandomVector = glm::normalize(glm::vec2(BravoMath::Rand(-1.0f, 1.0f), BravoMath::Rand(-1.0f, 1.0f)));
		PressureCompute->SetFloat2("RandomVector", RandomVector);

		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	PressureCompute->StopUsage();
}

void FluidSimulation3D::ExecuteRadixSort()
{
	for ( int32 iteration = 0; iteration < 4; ++iteration )
	{
		UpdateRadixIteration(iteration);

		RadixSortHistogramCompute->Use();
			glDispatchCompute(NumWorkGroups, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		RadixSortHistogramCompute->StopUsage();

		RadixSortCompute->Use();
			glDispatchCompute(NumWorkGroups, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		RadixSortCompute->StopUsage();
	}
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SortedParticlesSSBO);
}

void FluidSimulation3D::UpdateRadixIteration(int32 iteration)
{
	if (iteration == 0 || iteration == 2)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SortedParticlesSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, RadixTmpSSBO);
		
		RadixSortPushConstants.g_shift = iteration == 0 ? 0 : 16;
	}
	else if (iteration == 1 || iteration == 3)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SortedParticlesSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, RadixTmpSSBO);

		RadixSortPushConstants.g_shift = iteration == 1 ? 8 : 24;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, RadixSortConstantsUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(FluidSimulation3D::RadixSortConstants), &RadixSortPushConstants, GL_STATIC_DRAW);
}