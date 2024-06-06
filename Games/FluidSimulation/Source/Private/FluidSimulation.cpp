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

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("FluidPoint", BravoShaderLoadingParams("Shaders\\FluidPoint"));


	{
	Engine->GetInput()->OnMouseMoveDelegate.AddSP(Self<FluidSimulation>(), &FluidSimulation::OnMouseMove);
	Engine->GetInput()->OnMouseScrollDelegate.AddSP(Self<FluidSimulation>(), &FluidSimulation::OnMouseScroll);
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
	}


	return true;
}

void FluidSimulation::Render()
{
	if ( !Particles.size() )
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesSSBO);
	if ( CachedCount != Particles.size() )
	{
		CachedCount = Particles.size();
		glBufferData(GL_SHADER_STORAGE_BUFFER, Particles.size() * sizeof(Particle), Particles.data(), GL_DYNAMIC_DRAW);
	}
	else
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, Particles.size() * sizeof(Particle), Particles.data());
	}

	glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	glm::mat4 CameraView = camera->GetViewMatrix();

	glm::mat4 ViewProj = CameraProjection * CameraView;

	
	Shader->Use();

		Shader->SetMatrix4d("viewProj", ViewProj);
		Shader->SetVector1d("particleSize", ParticleSize);
		Shader->SetVector1d("maxSpeed", MaxVelocity);

		Shader->SetVector3d("Cold", Cold);
		Shader->SetVector3d("Middle", Middle);
		Shader->SetVector3d("Hot", Hot);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesSSBO);

		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Particles.size());
		glBindVertexArray(0);
	Shader->StopUsage();
}

void FluidSimulation::SpawnParticles()
{
	assert( ParentContainer != nullptr );

	ParticleGridSize.x = glm::ceil(WorldSize.x / ParticleSize * 2.0f);
    ParticleGridSize.y = glm::ceil(WorldSize.y / ParticleSize * 2.0f);

	if ( ParticleGridSize.x * ParticleGridSize.y < ParticlesCount )
		ParticlesCount = ParticleGridSize.x * ParticleGridSize.y;

	math.SetRadius(SmoothingRadius);
	
	bPaused = true;
	bHasStarted = false;

	Particles.clear();
	Particles.resize(ParticlesCount);
	
	OriginalPositions.clear();
	OriginalPositions.resize(ParticlesCount);
	
	ParticleIndicies.clear();
	ParticleIndicies.resize(ParticlesCount);

	Densities.clear();
	Densities.resize(ParticlesCount);

	NearDensities.clear();
	NearDensities.resize(ParticlesCount);

	PredictedPositions.clear();
	PredictedPositions.resize(ParticlesCount);

	WorldSize = ParentContainer->GetSize() * 2.0f;
	Grid.UpdateSize(WorldSize, ParticleSize);

	if ( ParticlesCount == 0 )
		return;

	if ( bRandomPositions )
	{
		for ( int32 i = 0; i < ParticlesCount; ++i )
		{
			glm::vec2 Range = ParentContainer->GetSize() - glm::vec2(ParticleSize*2.0f);
			float x = BravoMath::Rand(-Range.x, Range.x);
			float y = BravoMath::Rand(-Range.y, Range.y);
			Particles[i].Position = glm::vec2(x, y);
			OriginalPositions[i] = Particles[i].Position;
			PredictedPositions[i] = Particles[i].Position;

			ParticleIndicies[i] = i;
		}
	}
	else
	{
		float ParticleDiameter = 2 * ParticleSize;
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
				PredictedPositions[index] = Particles[index].Position;

				ParticleIndicies[index] = index;
				index++;
			}
		}
	}

	Grid.UpdateGrid(ParticleIndicies, PredictedPositions);
}

void FluidSimulation::Reset()
{
	assert( OriginalPositions.size() == Particles.size() );
	for ( int32 i = 0; i < OriginalPositions.size(); ++i )
	{
		Particles[i] = Particle();
		Particles[i].Position = OriginalPositions[i];
		PredictedPositions[i] = OriginalPositions[i];
	}
	Grid.UpdateGrid(ParticleIndicies, PredictedPositions);
	bHasStarted = false;
	bPaused = true;
}

void FluidSimulation::TogglePause()
{
	bPaused = !bPaused;
	if ( !bPaused )
		bHasStarted = true;
}

void FluidSimulation::OnInput_MOUSERIGHT(bool ButtonState, float DeltaTime)
{
	bMouseRight = ButtonState;
	if ( !bMouseLeft && !bMouseRight )
		InteractionForce = 0.0f;
}

void FluidSimulation::OnInput_MOUSELEFT(bool ButtonState, float DeltaTime)
{
	bMouseLeft = ButtonState;
	if ( !bMouseLeft && !bMouseRight )
		InteractionForce = 0.0f;
}

void FluidSimulation::OnInput_Space(bool ButtonState, float DeltaTime)
{
	TogglePause();
}

void FluidSimulation::OnMouseScroll(const glm::vec2& DeltaScroll, float DeltaTime)
{
	
}

void FluidSimulation::HightlightRelativeParticles(const glm::vec2& samplePoint)
{
	for ( int32 i : ParticleIndicies )
	{
		Particles[i].Hightligh = 0;
	}

	std::list<int32> RelatedParticles;
	Grid.GetRelatedParticles(samplePoint, RelatedParticles);
	for ( int32 i : RelatedParticles )
	{
		Particles[i].Hightligh = 1;
	}
}

void FluidSimulation::OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime)
{
	const glm::ivec2 vSize = Engine->GetViewport()->GetViewportSize();
	const glm::vec2 rPos = (glm::vec2(CurrentPosition.x / vSize.x, CurrentPosition.y / vSize.y ) * 2.0f) - glm::vec2(1.0f);

	const glm::vec2 halfWorldSize = WorldSize * 0.5f;

	const glm::vec2 mWorldPos = rPos * halfWorldSize;

	//float d, nd;
	//CalcDensity(mWorldPos, d, nd);
	//Log::LogMessage(ELog::Log, "{}, {}", d, nd);
	//HightlightRelativeParticles(mWorldPos);
	
	if ( bPaused || (!bMouseLeft && !bMouseRight) || (bMouseLeft && bMouseRight))
	{
		return;
	}

	InteractionLocation = mWorldPos;
	InteractionForce = bMouseLeft ? 1.0f : -1.0f;	
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
	assert(Densities.size() == Particles.size() && Densities.size() == ParticleIndicies.size() && Particles.size() == PredictedPositions.size() );
	

	// gravity position prediction
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, DeltaTime](int32 i)
		{
			Particles[i].Velocity += CalcExternalForces(i) * DeltaTime;
			PredictedPositions[i] = Particles[i].Position + (Particles[i].Velocity * 1.0f / 120.0f);
		});

	Grid.UpdateGrid(ParticleIndicies, PredictedPositions);

	// density
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this](int32 i)
		{
			CalcDensity(PredictedPositions[i], Densities[i], NearDensities[i]);
		});

	// pressure
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, DeltaTime](int32 i)
		{
			Particles[i].Velocity += CalcPressureForce(i) * DeltaTime;
			
		});

	// viscosity position, collision
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, DeltaTime](int32 i)
		{
			Particles[i].Velocity += CalcViscosity(i) * DeltaTime;

			if ( glm::length2(Particles[i].Velocity) > MaxVelocity*MaxVelocity )
				Particles[i].Velocity = glm::normalize(Particles[i].Velocity) * MaxVelocity;

			Particles[i].Position += Particles[i].Velocity * DeltaTime;
			glm::vec2 CollisionVelocity(0.0);
			if ( ParentContainer->CheckRoundCollision(Particles[i].Position, ParticleSize, CollisionVelocity) )
			{
				Particles[i].Velocity *= (CollisionVelocity * CollisionDamping);
			}
		});
}

void FluidSimulation::CalcDensity(const glm::vec2& samplePoint, float& Density, float& NearDensity) const
{
	Density = 0.0f;
	NearDensity = 0.0f;

	std::list<int32> RelatedParticles;
	Grid.GetRelatedParticles(samplePoint, RelatedParticles);
	if ( RelatedParticles.size() == 0 )
	{
		Grid.GetRelatedParticles(samplePoint, RelatedParticles);;
	}

	for ( int32 i : RelatedParticles )
	{
		float dst = glm::length(PredictedPositions[i] - samplePoint);
		Density += ParticleMass * DensityKernel(dst);
		NearDensity += ParticleMass * NearDensityKernel(dst);
	}
}

glm::vec2 FluidSimulation::CalcExternalForces(int32 i) const
{
	glm::vec2 gravityAcceleration = glm::vec2(BravoMath::Rand(-Gravity*0.02f, Gravity*0.02f), Gravity);
	if ( InteractionForce != 0.0f )
	{
		glm::vec2 offset = InteractionLocation - Particles[i].Position;
		float dst2 = glm::length2(offset);
		if ( dst2 < InteractionRadius*InteractionRadius )
		{
			float distance = glm::sqrt(dst2);
			float edgeT = (distance / InteractionRadius);
			float centreT = 1 - edgeT;
			glm::vec2 dirToCentre = offset / distance;

			float gravityWeight = 1 - (centreT * glm::saturate(InteractionAcceleration / 10));
			glm::vec2 acceleration = gravityAcceleration * gravityWeight + dirToCentre * centreT * InteractionAcceleration;
			acceleration -= Particles[i].Velocity * centreT;
			return acceleration * InteractionForce;
		}
	}
	return gravityAcceleration;
}

glm::vec2 FluidSimulation::CalcPressureForce(int32 pIndex) const
{
	glm::vec2 pressureForce = glm::vec2(0.0f);

	glm::vec2 randDir = glm::normalize(glm::vec2(BravoMath::Rand(-1.0f, 1.0f), BravoMath::Rand(-1.0f, 1.0f)));

	std::list<int32> RelatedParticles;
	Grid.GetRelatedParticles(PredictedPositions[pIndex], RelatedParticles);

	const float myDensity = Densities[pIndex];
	const float myPressure = DensityToPessure(myDensity);

	const float myNearDensity = NearDensities[pIndex];
	const float myNearPressure = NearDensityToPessure(myNearDensity);

	const float SmoothingRadius2 = SmoothingRadius * SmoothingRadius;
	
	for ( int32 otherIndex : RelatedParticles )
	{
		if ( pIndex == otherIndex ) continue;

		glm::vec2 offset = PredictedPositions[pIndex] - PredictedPositions[otherIndex];
		if ( glm::length2(offset) > SmoothingRadius2 ) continue;

		float dst = glm::length(offset);
		glm::vec2 dir = dst != 0.0f ? offset / dst : randDir;

		float otherDensity = Densities[otherIndex];
		float otherNearDensity = NearDensities[otherIndex];
		
		float otherPressure = DensityToPessure(otherDensity);
		float otherNearPressure = NearDensityToPessure(otherDensity);
		
		float sharedPressure = (myPressure + otherPressure) * 0.5f;
		
		float sharedNearPressure = (myNearPressure + otherNearPressure) * 0.5f;

		pressureForce += dir * DensityDerivative(dst) * sharedPressure / otherDensity;
		pressureForce += dir * NearDensityDerivative(dst) * sharedNearPressure / otherNearDensity;
	}
	if ( myDensity == 0.0f )
	{
		assert(false);
	}
	return pressureForce / myDensity;
}

glm::vec2 FluidSimulation::CalcViscosity(int32 pIndex) const
{
	glm::vec2 viscosityForce = glm::vec2(0.0f);
	std::list<int32> RelatedParticles;
	Grid.GetRelatedParticles(PredictedPositions[pIndex], RelatedParticles);
	const float SmoothingRadius2 = SmoothingRadius * SmoothingRadius;

	glm::vec2 myVelocity = Particles[pIndex].Velocity;

	for ( int32 otherIndex : RelatedParticles )
	{
		if ( pIndex == otherIndex ) continue;
		glm::vec2 offset = PredictedPositions[pIndex] - PredictedPositions[otherIndex];
		if ( glm::length2(offset) > SmoothingRadius2 ) continue;

		glm::vec2 otherVelocity = Particles[otherIndex].Velocity;

		float dst = glm::length(offset);

		viscosityForce += (otherVelocity - myVelocity) * ViscosityKernel(dst);
	}
	return viscosityForce * ViscosityFactor;
}



float FluidSimulation::DensityToPessure(float density) const
{
	return (density - TargetDensity) * Preassure;
}
float FluidSimulation::NearDensityToPessure(float density) const
{
	return density * NearPressureMultiplier;
}

float FluidSimulation::DensityKernel(float dst) const
{
	return math.SpikyKernelPow2(dst);
}

float FluidSimulation::NearDensityKernel(float dst) const
{
	return math.SpikyKernelPow3(dst);
}

float FluidSimulation::DensityDerivative(float dst) const
{
	return math.DerivativeSpikyPow2(dst);
}

float FluidSimulation::NearDensityDerivative(float dst) const
{
	return math.DerivativeSpikyPow3(dst);
}

float FluidSimulation::ViscosityKernel(float dst) const
{
	return math.SmoothingKernelPoly6(dst);
}