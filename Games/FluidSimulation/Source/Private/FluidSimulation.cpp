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



void FluidSimulation::UpdateMath()
{
	math.SetRadius(SmoothingRadius);
}

void FluidSimulation::SpawnParticles()
{
	assert( ParentContainer != nullptr );

	UpdateMath();
	
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

    SmoothingGridSize.x = glm::ceil(WorldSize.x / SmoothingRadius * 2.0f);
    SmoothingGridSize.y = glm::ceil(WorldSize.y / SmoothingRadius * 2.0f);

    ParticleGridSize.x = glm::ceil(WorldSize.x / ParticleSize * 2.0f);
    ParticleGridSize.y = glm::ceil(WorldSize.y / ParticleSize * 2.0f);

	if ( ParticleGridSize.x * ParticleGridSize.y < ParticlesCount )
		ParticlesCount = ParticleGridSize.x * ParticleGridSize.y;

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
		int32 gridWidth = static_cast<int32>(sqrt(ParticlesCount * aspectRatio));
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

	UpdateSpacialLookup();
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
	UpdateSpacialLookup();
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
	InteractionRadius = glm::min(InteractionRadius + InteractionStep * DeltaScroll.y, 0.0f);
}

void FluidSimulation::OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime)
{
	if ( bPaused || (!bMouseLeft && !bMouseRight) || (bMouseLeft && bMouseRight))
	{
		return;
	}


	const glm::ivec2 vSize = Engine->GetViewport()->GetViewportSize();
	const glm::vec2 rPos = (glm::vec2(CurrentPosition.x / vSize.x, CurrentPosition.y / vSize.y ) * 2.0f) - glm::vec2(1.0f);

	glm::vec2 halfWorldSize = WorldSize * 0.5f;

	const glm::vec2 mWorldPos = rPos * halfWorldSize;
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

glm::ivec2 FluidSimulation::GetCellCoord(const glm::vec2& Position) const
{
	glm::vec2 normilizedPos = Position + WorldSize * 0.5f;
	int32 cellX = static_cast<int32>(std::floor(normilizedPos.x / SmoothingRadius*0.5f));
    int32 cellY = static_cast<int32>(std::floor(normilizedPos.y / SmoothingRadius*0.5f));
	return glm::ivec2(cellX, cellY);
}

 bool FluidSimulation::GetCellHash(const glm::ivec2& Coords, CellHash& OutHash) const
{
	if (Coords.x < 0 || Coords.y < 0 || Coords.x >= SmoothingGridSize.x || Coords.y >= SmoothingGridSize.y)
	{
		return false;
	}
	int32 hash = Coords.y * SmoothingGridSize.x + Coords.x;
	int32 cellIndex = hash % CellOccupied.size();
	int32 pIndex = hash % ParticleIndicies.size();

	OutHash.CellIndex = cellIndex;
	OutHash.CellIndexP = pIndex;
	return true;
}

void FluidSimulation::UpdateSpacialLookup()
{
	//Lookup.clear();
	Lookup.resize(ParticleIndicies.size(), SpacialLookup());

	//StartIndices.clear();
	StartIndices.resize(ParticleIndicies.size(), -1);
	
	//CellOccupied.clear();
	CellOccupied.resize( SmoothingGridSize.x * SmoothingGridSize.y, false);

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this](int32 i)
		{
			CellHash cHash;
			if ( !GetCellHash(GetCellCoord(PredictedPositions[i]), cHash) )
				return;
			Lookup[i].cHash = cHash;
			Lookup[i].pIndex = i;
		});

	for ( const SpacialLookup& l : Lookup )
	{
		if ( l.cHash.CellIndex < 0 ) continue;
		CellOccupied[l.cHash.CellIndex] = true;
	}

	std::sort(
		Lookup.begin(),
		Lookup.end(),
		[this](const SpacialLookup& left, const SpacialLookup& right)
		{	
			return left.cHash.CellIndexP < right.cHash.CellIndexP;
		});

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this](int32 lookupIndex)
		{
			int32 CellHash = Lookup[lookupIndex].cHash.CellIndexP;
			int32 prevCellHash = lookupIndex != 0 ? Lookup[lookupIndex-1].cHash.CellIndexP : -1;
			if ( CellHash != prevCellHash )
			{
				StartIndices[CellHash] = lookupIndex;
			}
		});
}

void FluidSimulation::GetRelatedParticles(const glm::vec2& Position, std::list<int32>& OutParticles) const
{
	OutParticles.clear();

	std::list<glm::ivec2> offsets {
		glm::ivec2(0, 0),
		glm::ivec2(1, 0),
		glm::ivec2(-1, 0),
		glm::ivec2(0, 1),
		glm::ivec2(0, -1),
		glm::ivec2(1, 1),
		glm::ivec2(1, -1),
		glm::ivec2(-1, 1),
		glm::ivec2(-1, -1)
	};
	
	glm::ivec2 CellCoord = GetCellCoord(Position);
	for ( auto it : offsets )
		GetParticlesInCell(CellCoord+it, OutParticles);
}

void FluidSimulation::GetParticlesInCell(const glm::ivec2& CellCoords, std::list<int32>& OutParticles) const
{
	CellHash cHash;
	if ( !GetCellHash(CellCoords, cHash) )
		return;

	if ( !CellOccupied[cHash.CellIndex] )
		return;

	int32 CellKey = cHash.CellIndexP;

	if ( StartIndices.size() <= CellKey || CellKey < 0 )
		return;

	int32 StartIndex = StartIndices[CellKey];

	if ( StartIndex < 0 )
		return;
	
	for (  uint32 i = StartIndex; i < Lookup.size(); ++i)
	{
		if ( Lookup[i].cHash.CellIndexP != CellKey )
			return;

		OutParticles.push_back(Lookup[i].pIndex);
	}
}

void FluidSimulation::SimulationStep(float DeltaTime)
{
	assert(Densities.size() == Particles.size() && Densities.size() == ParticleIndicies.size() && Particles.size() == PredictedPositions.size() );

	
	UpdateSpacialLookup();

	glm::vec2 gravityAcceleration = glm::vec2(BravoMath::Rand(-Gravity*0.2f, Gravity*0.2f), Gravity);

	// gravity position prediction
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, gravityAcceleration, DeltaTime](int32 i)
		{
			glm::vec2 acceleration = gravityAcceleration;
			if ( InteractionForce != 0.0f )
			{
				glm::vec2 offset = InteractionLocation - Particles[i].Position;
				float dst2 = glm::length2(offset);
				if ( dst2 < InteractionRadius )
				{
					float distance = glm::sqrt(dst2);
					glm::vec2 direction = offset / distance;

					float nDistance = (1.0f - distance / InteractionRadius);

					float gravityW = (nDistance * glm::saturate(InteractionAcceleration / 10.0f));
					acceleration = gravityAcceleration * gravityW + direction * nDistance * InteractionAcceleration;
					acceleration += Particles[i].Velocity * nDistance;
					acceleration *= InteractionForce;
				}
			}

			Particles[i].Velocity += acceleration * DeltaTime;
			PredictedPositions[i] = Particles[i].Position + (Particles[i].Velocity * 1.0f / 120.0f);
		});

	// density
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, gravityAcceleration](int32 i)
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

float FluidSimulation::DensityToPessure(float density) const
{
	return (density - TargetDensity) * Preassure;
}
float FluidSimulation::NearDensityToPessure(float density) const
{
	return density * -NearPressureMultiplier;
}

glm::vec2 FluidSimulation::CalcPressureForce(int32 pIndex) const
{
	glm::vec2 pressureForce = glm::vec2(0.0f);

	glm::vec2 randDir = glm::normalize(glm::vec2((float)(rand()) / (float)(RAND_MAX), (float)(rand()) / (float)(RAND_MAX)));

	std::list<int32> RelatedParticles;
	GetRelatedParticles(PredictedPositions[pIndex], RelatedParticles);

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
		float otherPressure = DensityToPessure(otherDensity);
		float sharedPressure = (myPressure + otherPressure)  / (2.0f*otherDensity);
		
		float otherNearDensity = NearDensities[otherIndex];
		float otherNearPressure = NearDensityToPessure(otherDensity);
		float sharedNearPressure = (myNearPressure + otherNearPressure)  / (2.0f*otherNearDensity);

		pressureForce += dir * DensityDerivative(dst) * sharedPressure;
		pressureForce += dir * NearDensityDerivative(dst) * sharedNearPressure;
	}
	return pressureForce * 10000.0f / myDensity;
}

glm::vec2 FluidSimulation::CalcViscosity(int32 pIndex) const
{
	glm::vec2 viscosityForce = glm::vec2(0.0f);
	std::list<int32> RelatedParticles;
	GetRelatedParticles(PredictedPositions[pIndex], RelatedParticles);
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

void FluidSimulation::CalcDensity(const glm::vec2& samplePoint, float& Density, float& NearDensity) const
{
	Density = 0.0f;
	NearDensity = 0.0f;

	std::list<int32> RelatedParticles;
	GetRelatedParticles(samplePoint, RelatedParticles);

	for ( int32& i : RelatedParticles )
	{
		float dst = glm::length(PredictedPositions[i] - samplePoint);
		Density += ParticleMass * DensityKernel(dst);
		NearDensity += ParticleMass * NearDensityKernel(dst);
	}
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