#include "FluidSimulation.h"
#include "FluidContainer.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoCamera.h"
#include "BravoInput.h"
#include "BravoViewport.h"

bool FluidSimulation::Initialize_Internal()
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

	
	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

	RenderShader = AssetManager->FindOrLoad<BravoShaderAsset>("FluidPoint", BravoShaderLoadingParams("FluidPoint"));

	GridHashingCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidGridHashing", BravoShaderLoadingParams("Compute\\FluidGridHashing"));
	
	RadixSortCompute = AssetManager->FindOrLoad<BravoShaderAsset>("RadixSort", BravoShaderLoadingParams("Compute\\ThirdParty\\multi_radixsort"));
	RadixSortHistogramCompute = AssetManager->FindOrLoad<BravoShaderAsset>("RadixSortHistogram", BravoShaderLoadingParams("Compute\\ThirdParty\\multi_radixsort_histograms"));

	FluidStartingIndiciesCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidStartingIndicies", BravoShaderLoadingParams("Compute\\FluidStartingIndicies"));
	PressureCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidPressure", BravoShaderLoadingParams("Compute\\FluidPressure"));

	{
		Engine->GetInput()->OnMouseMoveDelegate.AddSP(Self<FluidSimulation>(), &FluidSimulation::OnMouseMove);
	{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_MOUSE_BUTTON_RIGHT;
		subscription.SubscribedType = EKeySubscriptionType::Hold | EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<FluidSimulation>(), &FluidSimulation::OnInput_MOUSERIGHT);
		Engine->GetInput()->SubscribeKey(subscription);
	}
	{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_MOUSE_BUTTON_LEFT;
		subscription.SubscribedType = EKeySubscriptionType::Hold | EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<FluidSimulation>(), &FluidSimulation::OnInput_MOUSELEFT);
		Engine->GetInput()->SubscribeKey(subscription);
	}
	{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_SPACE;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<FluidSimulation>(), &FluidSimulation::OnInput_Space);
		Engine->GetInput()->SubscribeKey(subscription);
	}
	{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_R;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<FluidSimulation>(), &FluidSimulation::OnInput_R);
		Engine->GetInput()->SubscribeKey(subscription);
	}
	}

	return true;
}

void FluidSimulation::FillBuffers()
{
	NumWorkGroups = (Particles.size() + 255) / 256;


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Particles.size() * sizeof(Particle), Particles.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SortedParticlesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Particles.size() * sizeof(uint32) * 2, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SortedParticlesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RadixTmpSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Particles.size() * sizeof(uint32) * 2, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, RadixTmpSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RadixHistogramSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NumWorkGroups * 256 * sizeof(uint32), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, RadixHistogramSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, StartIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Particles.size() * sizeof(uint32), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, StartIndicesSSBO);

	glBindBuffer(GL_UNIFORM_BUFFER, RadixSortConstantsUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, RadixSortConstantsUBO);

	RadixSortPushConstants.g_num_elements = Particles.size();
	RadixSortPushConstants.g_num_workgroups = NumWorkGroups;
	RadixSortPushConstants.g_num_blocks_per_workgroup = 256;
}

void FluidSimulation::UpdateShaderUniformParams()
{
	FluidStartingIndiciesCompute->Use();
		FluidStartingIndiciesCompute->SetInt("ParticleCount", ParticleCount);
	FluidStartingIndiciesCompute->StopUsage();

	// hashing
	GridHashingCompute->Use();
		GridHashingCompute->SetFloat2("WorldSize", WorldSize);
		GridHashingCompute->SetFloat1("SmoothingRadius", SmoothingRadius);
		GridHashingCompute->SetInt("ParticleCount", ParticleCount);
	GridHashingCompute->StopUsage();
	
	// pressure
	PressureCompute->Use();
		PressureCompute->SetInt("ParticleCount", ParticleCount);

		PressureCompute->SetFloat2("WorldSize", WorldSize);
		PressureCompute->SetFloat2("ContainerSize", ParentContainer->GetSize());
		PressureCompute->SetFloat1("MaxVelocity", MaxVelocity);

		PressureCompute->SetFloat1("ParticleMass", ParticleMass);
		PressureCompute->SetFloat1("ParticleRadius", ParticleRadius);
		PressureCompute->SetFloat1("SmoothingRadius", SmoothingRadius);
		PressureCompute->SetFloat1("TargetDensity", TargetDensity);
	
		PressureCompute->SetFloat1("Preassure", Preassure);
		PressureCompute->SetFloat1("NearPreassure", NearPreassure);
		PressureCompute->SetFloat1("ViscosityFactor", ViscosityFactor);
		PressureCompute->SetFloat1("CollisionDamping", CollisionDamping);

		PressureCompute->SetFloat1("GravityForce", Gravity);
		PressureCompute->SetFloat1("InteractionForce", InteractionForce);
		PressureCompute->SetFloat2("InteractionLocation", InteractionLocation);
		PressureCompute->SetFloat1("InteractionRadius", InteractionRadius);

		const float DensityScale		= 6.0f  / (1.0f * (glm::pi<float>() * glm::pow(SmoothingRadius, 4)));
		const float NearDensityScale	= 10.0f / (1.0f * (glm::pi<float>() * glm::pow(SmoothingRadius, 5)));
		const float PressureScale		= 12.0f / (1.0f * (glm::pi<float>() * glm::pow(SmoothingRadius, 4)));
		const float NearPressureScale	= 30.0f / (1.0f * (glm::pi<float>() * glm::pow(SmoothingRadius, 5)));
		const float ViscosityScale		= 4.0f  / (1.0f * (glm::pi<float>() * glm::pow(SmoothingRadius, 8)));
		
		PressureCompute->SetFloat1("DensityScale", DensityScale);
		PressureCompute->SetFloat1("NearDensityScale", NearDensityScale);
		PressureCompute->SetFloat1("PressureScale", PressureScale);
		PressureCompute->SetFloat1("NearPressureScale", NearPressureScale);
		PressureCompute->SetFloat1("ViscosityScale", ViscosityScale);
	PressureCompute->StopUsage();
}

void FluidSimulation::Render()
{
	if ( !Particles.size() )
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;
	
	glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	glm::mat4 CameraView = camera->GetViewMatrix();

	glm::mat4 ViewProj = CameraProjection * CameraView;
	
	RenderShader->Use();

		RenderShader->SetMatrix4d("viewProj", ViewProj);
		RenderShader->SetFloat1("particleSize", ParticleRadius);
		RenderShader->SetFloat1("maxSpeed", MaxVelocity);

		RenderShader->SetFloat3("Cold", Cold);
		RenderShader->SetFloat3("Middle", Middle);
		RenderShader->SetFloat3("Hot", Hot);
				

		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Particles.size());
		glBindVertexArray(0);

	RenderShader->StopUsage();
}

void FluidSimulation::SpawnParticles()
{
	assert( ParentContainer != nullptr );

	WorldSize = ParentContainer->GetSize() * 2.0f;

	glm::ivec2 ParticleGridSize;
	ParticleGridSize.x = glm::ceil(WorldSize.x / ParticleRadius * 2.0f);
    ParticleGridSize.y = glm::ceil(WorldSize.y / ParticleRadius * 2.0f);

	if ( ParticleGridSize.x * ParticleGridSize.y < ParticleCount )
		ParticleCount = ParticleGridSize.x * ParticleGridSize.y;


	bPaused = true;
	bHasStarted = false;

	Particles.clear();
	Particles.resize(ParticleCount);
	
	OriginalPositions.clear();
	OriginalPositions.resize(ParticleCount);
	
	

	if ( ParticleCount == 0 )
		return;

	
	if ( bRandomPositions )
	{
		for ( int32 i = 0; i < ParticleCount; ++i )
		{
			glm::vec2 Range = ParentContainer->GetSize() - glm::vec2(ParticleRadius*2.0f);
			float x = BravoMath::Rand(-Range.x, Range.x);
			float y = BravoMath::Rand(-Range.y, Range.y);
			Particles[i].Position = glm::vec2(x, y);
			Particles[i].PredictedPosition = Particles[i].Position;
			OriginalPositions[i] = Particles[i].Position;
		}
	}
	else
	{
		float ParticleDiameter = 2 * ParticleRadius;
		float aspectRatio = WorldSize.x / WorldSize.y;
		int32 gridWidth = static_cast<int32>(glm::ceil(sqrt(ParticleCount * aspectRatio)));
		int32 gridHeight = (ParticleCount + gridWidth - 1) / gridWidth;

		// Adjust ParticleCount if it exceeds the capacity of the grid
		ParticleCount = std::min(ParticleCount, gridWidth * gridHeight);

		// Calculate the offsets to center the grid around the origin
		float offsetX = -(gridWidth - 1) * ParticleDiameter / 2;
		float offsetY = -(gridHeight - 1) * ParticleDiameter / 2;

		int32 index = 0;
		for (int32 i = 0; i < gridWidth && index < ParticleCount; ++i)
		{
			for (int32 j = 0; j < gridHeight && index < ParticleCount; ++j)
			{
				float x = i * ParticleDiameter + offsetX;
				float y = j * ParticleDiameter + offsetY;

				Particles[index].Position = glm::vec2(x, y);
				Particles[index].PredictedPosition = Particles[index].Position;
				OriginalPositions[index] = Particles[index].Position;
				index++;
			}
		}
	}

	CachedParticlesCount = ParticleCount;

	FillBuffers();
	UpdateShaderUniformParams();
	SimulationStep(0.0f);
}

void FluidSimulation::Reset()
{
	SpawnParticles();
}

void FluidSimulation::TogglePause()
{
	bPaused = !bPaused;
	if ( !bPaused )
		bHasStarted = true;
}

void FluidSimulation::OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime)
{
	if ( bMouseRight )
		SetMouseForce(CurrentPosition, -1.0f);
	if ( bMouseLeft )
		SetMouseForce(CurrentPosition, 1.0f);
}

void FluidSimulation::OnInput_MOUSERIGHT(bool ButtonState, float DeltaTime)
{
	bMouseRight = ButtonState;
	if ( bMouseRight )
		SetMouseForce(Engine->GetInput()->GetMousePosition(), -1.0f);
	else if ( !bMouseLeft )
		InteractionForce = 0.0f;
}

void FluidSimulation::OnInput_MOUSELEFT(bool ButtonState, float DeltaTime)
{
	bMouseLeft = ButtonState;
	if ( bMouseLeft )
		SetMouseForce(Engine->GetInput()->GetMousePosition(), 1.0f);
	else if ( !bMouseRight )
		InteractionForce = 0.0f;
}

void FluidSimulation::OnInput_Space(bool ButtonState, float DeltaTime)
{
	TogglePause();	
}
void FluidSimulation::OnInput_R(bool ButtonState, float DeltaTime)
{
	Reset();
}

void FluidSimulation::SetMouseForce(const glm::vec2& MouseLocation, float Dir)
{
	const glm::ivec2 vSize = Engine->GetViewport()->GetViewportSize();
	const glm::vec2 rPos = (glm::vec2(MouseLocation.x / vSize.x, MouseLocation.y / vSize.y ) * 2.0f) - glm::vec2(1.0f);

	const glm::vec2 halfWorldSize = WorldSize * 0.5f;

	const glm::vec2 mWorldPos = rPos * halfWorldSize;

	InteractionLocation = mWorldPos;
	InteractionForce = InteractionAcceleration * Dir;
}


///////////////////////////////////////////////

void FluidSimulation::Tick(float DeltaTime)
{
	if ( bPaused ) return;
	for ( int32 i = 0; i < StepsPerTick; ++i )
		SimulationStep(DeltaTime / StepsPerTick);
}

void FluidSimulation::SimulationStep(float DeltaTime)
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

		PressureCompute->SetFloat1("SimulationTimeStep", DeltaTime);
		PressureCompute->SetFloat1("InteractionForce", InteractionForce);
		PressureCompute->SetFloat2("InteractionLocation", InteractionLocation);
		PressureCompute->SetFloat1("InteractionRadius", InteractionRadius);

		glm::vec2 RandomVector = glm::vec2(1.0f);//glm::normalize(glm::vec2(BravoMath::Rand(-1.0f, 1.0f), BravoMath::Rand(-1.0f, 1.0f)));
		PressureCompute->SetFloat2("RandomVector", RandomVector);

		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	PressureCompute->StopUsage();
}

void FluidSimulation::ExecuteRadixSort()
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

void FluidSimulation::UpdateRadixIteration(int32 iteration)
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
	glBufferData(GL_UNIFORM_BUFFER, sizeof(FluidSimulation::RadixSortConstants), &RadixSortPushConstants, GL_STATIC_DRAW);
}