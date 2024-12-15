#include "FluidSimulation3D.h"
#include "BravoBoundingBox.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoCamera.h"
#include "BravoInput.h"
#include "BravoViewport.h"
#include "BravoStaticMeshAsset.h"
#include "FluidPostProcess.h"

bool FluidSimulation3D::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	static float vertices[] = {
        // positions   // texCoords
        -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  1.0f, 0.0f,

        -0.5f,  0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f
    };

	glGenVertexArrays(1, &ParticleVAO);
	glGenBuffers(1, &ParticleVBO);
    glBindVertexArray(ParticleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ParticleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenBuffers(1, &ParticlesSSBO);
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
	BoundingBox->SetScale(SimulationBounds);
	BoundingBox->OnTransformUpdated.AddSP(Self<FluidSimulation3D>(), &FluidSimulation3D::OnBoundingBoxTransofrmUpdated);
	
	RenderShader = AssetManager->FindOrLoad<BravoShaderAsset>("FluidParticleShader", BravoShaderLoadingParams("FluidParticle3D"));

	ParticleGenerationCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidGenerateParticles", BravoShaderLoadingParams("Compute\\FluidGenerateParticles"));

	GridHashingCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidGridHashing", BravoShaderLoadingParams("Compute\\FluidGridHashing"));
	
	RadixSortCompute = AssetManager->FindOrLoad<BravoShaderAsset>("RadixSort", BravoShaderLoadingParams("Compute\\ThirdParty\\multi_radixsort"));
	RadixSortHistogramCompute = AssetManager->FindOrLoad<BravoShaderAsset>("RadixSortHistogram", BravoShaderLoadingParams("Compute\\ThirdParty\\multi_radixsort_histograms"));

	FluidStartingIndiciesCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidStartingIndicies", BravoShaderLoadingParams("Compute\\FluidStartingIndicies"));
	PressureCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidPressure", BravoShaderLoadingParams("Compute\\FluidPressure"));

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
	NumWorkGroups = (ParticleCount + 255) / 256;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ParticleCount * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SortedParticlesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ParticleCount * sizeof(uint32) * 2, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SortedParticlesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RadixTmpSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ParticleCount * sizeof(uint32) * 2, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, RadixTmpSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RadixHistogramSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NumWorkGroups * 256 * sizeof(uint32), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, RadixHistogramSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, StartIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ParticleCount * sizeof(uint32), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, StartIndicesSSBO);

	glBindBuffer(GL_UNIFORM_BUFFER, RadixSortConstantsUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, RadixSortConstantsUBO);

	RadixSortPushConstants.g_num_elements = ParticleCount;
	RadixSortPushConstants.g_num_workgroups = NumWorkGroups;
	RadixSortPushConstants.g_num_blocks_per_workgroup = 256;
}

void FluidSimulation3D::OnBoundingBoxTransofrmUpdated(const class IBravoTransformable*)
{
	glm::vec3 ContainerSize = BoundingBox->GetScale();

	GridHashingCompute->Use();
		GridHashingCompute->SetFloat3("WorldSize", ContainerSize);
	GridHashingCompute->StopUsage();

	PressureCompute->Use();
		PressureCompute->SetFloat3("WorldSize", ContainerSize);
		
		PressureCompute->SetMatrix4d("BoundingBoxModel",
			BoundingBox->GetTransform().GetTransformMatrix());

		PressureCompute->SetMatrix4d("InverseBoundingBoxModel", glm::inverse(
			BoundingBox->GetTransform().GetTransformMatrix()));
	PressureCompute->StopUsage();
}

void FluidSimulation3D::UpdateShaderUniformParams()
{
	glm::vec3 ContainerSize = BoundingBox->GetScale();

	SmoothingRadius = ParticleRadius * 4.0f;
	NearPressureFactor = TargetDensity * 0.01f;


	// hashing
	GridHashingCompute->Use();
		GridHashingCompute->SetFloat3("WorldSize", ContainerSize);
		GridHashingCompute->SetFloat1("SmoothingRadius", SmoothingRadius);
		GridHashingCompute->SetInt("ParticleCount", ParticleCount);
	GridHashingCompute->StopUsage();

	// starting indicies
	FluidStartingIndiciesCompute->Use();
		FluidStartingIndiciesCompute->SetInt("ParticleCount", ParticleCount);
	FluidStartingIndiciesCompute->StopUsage();
	
	// pressure
	PressureCompute->Use();
		PressureCompute->SetInt("ParticleCount", ParticleCount);

		PressureCompute->SetFloat1("GravityForce", Gravity);
		PressureCompute->SetFloat3("WorldSize", ContainerSize);
		PressureCompute->SetMatrix4d("BoundingBoxModel",
			BoundingBox->GetTransform().GetTransformMatrix());
		PressureCompute->SetMatrix4d("InverseBoundingBoxModel", glm::inverse(
			BoundingBox->GetTransform().GetTransformMatrix()));
		PressureCompute->SetFloat1("MaxVelocity", MaxVelocity);

		PressureCompute->SetFloat1("ParticleMass", ParticleMass);
		PressureCompute->SetFloat1("ParticleRadius", ParticleRadius);
		PressureCompute->SetFloat1("SmoothingRadius", SmoothingRadius);
		PressureCompute->SetFloat1("SmoothingRadius2", SmoothingRadius*SmoothingRadius);
		PressureCompute->SetFloat1("TargetDensity", TargetDensity);
	
		PressureCompute->SetFloat1("PressureFactor", PressureFactor);
		PressureCompute->SetFloat1("ViscosityFactor", ViscosityFactor);
		PressureCompute->SetFloat1("CollisionDamping", CollisionDamping);

		DensityScale		= 15.0f   / (2.0f  * (glm::pi<float>() * glm::pow(SmoothingRadius, 5)));
		NearDensityScale	= 15.0f   / (1.0f  * (glm::pi<float>() * glm::pow(SmoothingRadius, 6)));
		PressureScale		= 15.0f   / (1.0f  * (glm::pi<float>() * glm::pow(SmoothingRadius, 5)));
		NearPressureScale	= 45.0f   / (1.0f  * (glm::pi<float>() * glm::pow(SmoothingRadius, 6)));
		ViscosityScale		= 315.0f  / (64.0f * (glm::pi<float>() * glm::pow(SmoothingRadius, 9)));
		
		PressureCompute->SetFloat1("DensityScale", DensityScale);
		PressureCompute->SetFloat1("NearDensityScale", NearDensityScale);
		PressureCompute->SetFloat1("PressureScale", PressureScale);
		PressureCompute->SetFloat1("NearPressureScale", NearPressureScale);
		PressureCompute->SetFloat1("ViscosityScale", ViscosityScale);
	PressureCompute->StopUsage();
}

void FluidSimulation3D::Render()
{
	return;
	if ( ParticleCount <= 0 && bReadyToRender)
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

		RenderShader->SetMatrix4d("view", CameraView);
		RenderShader->SetMatrix4d("viewProj", ViewProj);
		RenderShader->SetFloat1("particleSize", ParticleRadius);
		RenderShader->SetFloat1("maxSpeed", MaxVelocity);

		RenderShader->SetFloat3("Cold", Cold);
		RenderShader->SetFloat3("Middle", Middle);
		RenderShader->SetFloat3("Hot", Hot);

		RenderShader->SetFloat1("TargetDensity", TargetDensity);
				
		glBindVertexArray(ParticleVAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, ParticleCount);
			//glDrawArrays(GL_POINTS, 0, ParticleCount);
		glBindVertexArray(0);

	RenderShader->StopUsage();
}

void FluidSimulation3D::Reset()
{
	BoundingBox->SetScale(SimulationBounds);

	FillBuffers();
	UpdateShaderUniformParams();
	
	bPaused = true;

	ParticleGenerationCompute->Use();
		// update time step	
		ParticleGenerationCompute->SetInt("ParticleCount", ParticleCount);
		float SpawnParticleDist = glm::pow((ParticleMass / TargetDensity), 1.0f / 3.0f);
		ParticleGenerationCompute->SetFloat1("Offset", SpawnParticleDist);

		BravoTransform spawnBoxTransform = BoundingBox->GetTransform();
		spawnBoxTransform.SetScale(spawnBoxTransform.GetScale() * glm::vec3(0.75f, 0.75f, 1.0f));

		ParticleGenerationCompute->SetMatrix4d("BoundingBox", spawnBoxTransform.GetTransformMatrix());
		
		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	ParticleGenerationCompute->StopUsage();

	SimulationStep(0.0f);

	bReadyToRender = true;

	bStarted = false;
}

void FluidSimulation3D::TogglePause()
{
	bPaused = !bPaused;
	if ( !bPaused )
	{
		if ( !bStarted )
		{
			SimulationStartTime = LifeTime;
		}
		bStarted = true;
	}
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
	//float elapsed = LifeTime - SimulationStartTime;
	//float amplitude = 0.75f * SimulationBounds.x;
	//if ( elapsed > 10.0f )
	//{
	//	static float start = LifeTime;
	//	float moveT = start - LifeTime;
	//	//BoundingBox->SetRotation(glm::vec3(0.0f, LifeTime*60.0f, 0.0f));
	//	//glm::vec3(150.0f, 75.0f, 25.0f)
	//	glm::vec3 newScale = SimulationBounds + glm::vec3(sin(moveT)*amplitude, 0.0f, 0.0f);
	//	BoundingBox->SetScale(newScale);
	//	//BoundingBox->SetLocation(glm::vec3(sin(moveT) * amplitude * 0.5f, 0.0f, 0.0f));
	//}

	for ( int32 i = 0; i < StepsPerTick; ++i )
		SimulationStep(DeltaTime / StepsPerTick);
}

void FluidSimulation3D::SimulationStep(float DeltaTime)
{
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