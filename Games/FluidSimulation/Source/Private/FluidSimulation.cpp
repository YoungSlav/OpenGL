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
	
	glGenBuffers(1, &StartIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, StartIndicesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	auto AssetManager = Engine->GetAssetManager();

	RenderShader = AssetManager->FindOrLoad<BravoShaderAsset>("FluidPoint", BravoShaderLoadingParams("Shaders\\FluidPoint"));

	ExternalForcesCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidExternalForces", BravoShaderLoadingParams("Shaders\\Compute\\FluidExternalForces"));
	GridHashingCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidGridHashing", BravoShaderLoadingParams("Shaders\\Compute\\FluidGridHashing"));
	GridSortingCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidGridSorting", BravoShaderLoadingParams("Shaders\\Compute\\FluidGridSorting"));
	PressureCompute = AssetManager->FindOrLoad<BravoShaderAsset>("FluidPressure", BravoShaderLoadingParams("Shaders\\Compute\\FluidPressure"));

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
	NumWorkGroups = (Particles.size() + 1023) / 1024;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Particles.size() * sizeof(Particle), Particles.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SortedParticlesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Particles.size() * sizeof(uint32) * 3, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SortedParticlesSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, StartIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Particles.size() * sizeof(uint32), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, StartIndicesSSBO);
}

void FluidSimulation::UpdateShaderUniformParams()
{
	// external force
	ExternalForcesCompute->Use();
		ExternalForcesCompute->SetVector1d("GravityForce", Gravity);
		ExternalForcesCompute->SetVector1d("InteractionForce", InteractionForce);
		ExternalForcesCompute->SetVector2d("InteractionLocation", InteractionLocation);
		ExternalForcesCompute->SetVector1d("InteractionRadius", InteractionRadius);
	ExternalForcesCompute->StopUsage();

	// hashing
	GridHashingCompute->Use();
		GridHashingCompute->SetVector2d("WorldSize", WorldSize);
		GridHashingCompute->SetVector1d("SmoothingRadius", SmoothingRadius);
	GridHashingCompute->StopUsage();
	// sorting
	
	// pressure
	PressureCompute->Use();
		PressureCompute->SetVector2d("WorldSize", WorldSize);
		PressureCompute->SetVector2d("ContainerSize", ParentContainer->GetSize());
		PressureCompute->SetVector1d("MaxVelocity", MaxVelocity);

		PressureCompute->SetVector1d("ParticleMass", ParticleMass);
		PressureCompute->SetVector1d("ParticleRadius", ParticleRadius);
		PressureCompute->SetVector1d("SmoothingRadius", SmoothingRadius);
		PressureCompute->SetVector1d("TargetDensity", TargetDensity);
	
		PressureCompute->SetVector1d("Preassure", Preassure);
		PressureCompute->SetVector1d("NearPressureMultiplier", NearPressureMultiplier);
		PressureCompute->SetVector1d("ViscosityFactor", ViscosityFactor);
		PressureCompute->SetVector1d("CollisionDamping", CollisionDamping);

		const float ViscosityScale		= 4.0f  / (glm::pi<float>() * glm::pow(SmoothingRadius, 8));
		const float NearDensityScale	= 10.0f / (glm::pi<float>() * glm::pow(SmoothingRadius, 5));
		const float DensityScale		= 6.0f  / (glm::pi<float>() * glm::pow(SmoothingRadius, 4));
		const float NearPressureScale	= 30.0f / (glm::pi<float>() * glm::pow(SmoothingRadius, 5));
		const float PressureScale		= 12.0f / (glm::pi<float>() * glm::pow(SmoothingRadius, 4));

		PressureCompute->SetVector1d("DensityScale", DensityScale);
		PressureCompute->SetVector1d("NearDensityScale", NearDensityScale);
		PressureCompute->SetVector1d("PressureScale", PressureScale);
		PressureCompute->SetVector1d("NearPressureScale", NearPressureScale);
		PressureCompute->SetVector1d("ViscosityScale", ViscosityScale);
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
		RenderShader->SetVector1d("particleSize", ParticleRadius);
		RenderShader->SetVector1d("maxSpeed", MaxVelocity);

		RenderShader->SetVector3d("Cold", Cold);
		RenderShader->SetVector3d("Middle", Middle);
		RenderShader->SetVector3d("Hot", Hot);
				

		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Particles.size());
		glBindVertexArray(0);

	RenderShader->StopUsage();
}

void FluidSimulation::SpawnParticles()
{
	assert( ParentContainer != nullptr );

	glm::ivec2 ParticleGridSize;
	ParticleGridSize.x = glm::ceil(WorldSize.x / ParticleRadius * 2.0f);
    ParticleGridSize.y = glm::ceil(WorldSize.y / ParticleRadius * 2.0f);

	if ( ParticleGridSize.x * ParticleGridSize.y < ParticlesCount )
		ParticlesCount = ParticleGridSize.x * ParticleGridSize.y;


	bPaused = true;
	bHasStarted = false;

	Particles.clear();
	Particles.resize(ParticlesCount);
	
	OriginalPositions.clear();
	OriginalPositions.resize(ParticlesCount);
	
	WorldSize = ParentContainer->GetSize() * 2.0f;

	if ( ParticlesCount == 0 )
		return;

	
	if ( bRandomPositions )
	{
		for ( int32 i = 0; i < ParticlesCount; ++i )
		{
			glm::vec2 Range = ParentContainer->GetSize() - glm::vec2(ParticleRadius*2.0f);
			float x = BravoMath::Rand(-Range.x, Range.x);
			float y = BravoMath::Rand(-Range.y, Range.y);
			Particles[i].Position = glm::vec2(x, y);
			OriginalPositions[i] = Particles[i].Position;
		}
	}
	else
	{
		float ParticleDiameter = 2 * ParticleRadius;
		float aspectRatio = WorldSize.x / WorldSize.y;
		int32 gridWidth = static_cast<int32>(glm::ceil(sqrt(ParticlesCount * aspectRatio)));
		int32 gridHeight = (ParticlesCount + gridWidth - 1) / gridWidth;

		// Adjust ParticleCount if it exceeds the capacity of the grid
		ParticlesCount = std::min(ParticlesCount, gridWidth * gridHeight);

		// Calculate the offsets to center the grid around the origin
		float offsetX = -(gridWidth - 1) * ParticleDiameter / 2;
		float offsetY = -(gridHeight - 1) * ParticleDiameter / 2;

		int32 index = 0;
		for (int32 i = 0; i < gridWidth && index < ParticlesCount; ++i)
		{
			for (int32 j = 0; j < gridHeight && index < ParticlesCount; ++j)
			{
				float x = i * ParticleDiameter + offsetX;
				float y = j * ParticleDiameter + offsetY;

				Particles[index].Position = glm::vec2(x, y);
				OriginalPositions[index] = Particles[index].Position;
				index++;
			}
		}
	}

	CachedParticlesCount = ParticlesCount;

	FillBuffers();
	UpdateShaderUniformParams();
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
//	TogglePause();
	SimulationTarget++;
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
	//if ( SimulationTarget <= CurrentSimulationStep )
	//	return;
	//CurrentSimulationStep++;

	if ( bPaused ) return;

	ExternalForcesCompute->Use();
		// update time step
		ExternalForcesCompute->SetVector1d("SimulationTimeStep", DeltaTime);
		ExternalForcesCompute->SetVector1d("InteractionForce", InteractionForce);
		ExternalForcesCompute->SetVector2d("InteractionLocation", InteractionLocation);
		ExternalForcesCompute->SetVector1d("InteractionRadius", InteractionRadius);
		
		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	ExternalForcesCompute->StopUsage();
	
	GridHashingCompute->Use();
		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	GridHashingCompute->StopUsage();

	struct SortedParticle
	{
		uint32 CellHash;
		uint32 CellIndex;
		uint32 pIndex;
	};

	// TODO: move sorting to compute shader
	{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SortedParticlesSSBO);
		std::vector<SortedParticle> sortedParticles(CachedParticlesCount);
		void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
		if (ptr)
		{
			memcpy(sortedParticles.data(), ptr, CachedParticlesCount * sizeof(SortedParticle));

			std::sort(sortedParticles.begin(), sortedParticles.end(),
				[](const SortedParticle& a, const SortedParticle& b)
				{
					return a.CellIndex < b.CellIndex;
				});
			memcpy(ptr, sortedParticles.data(), CachedParticlesCount * sizeof(SortedParticle));
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	
	GridSortingCompute->Use();
		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	GridSortingCompute->StopUsage();

	PressureCompute->Use();
		// update time step
		PressureCompute->SetVector1d("SimulationTimeStep", DeltaTime);

		glDispatchCompute(NumWorkGroups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	PressureCompute->StopUsage();
}