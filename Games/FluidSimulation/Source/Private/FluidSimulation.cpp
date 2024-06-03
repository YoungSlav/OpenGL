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


	Engine->GetInput()->OnMouseMoveDelegate.AddSP(Self<FluidSimulation>(), &FluidSimulation::OnMouseMove);


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
		
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesSSBO);

		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Particles.size());
		glBindVertexArray(0);

	Shader->StopUsage();
}



void FluidSimulation::SpawnParticles()
{
	assert( ParentContainer != nullptr );
	
	bPaused = true;
	bHasStarted = false;

	Particles.clear();
	Particles.resize(ParticlesCount);
	
	OriginalPositions.clear();
	OriginalPositions.resize(ParticlesCount);
	
	ParticleIndicies.clear();
	ParticleIndicies.resize(ParticlesCount);

	ParticleDensities.clear();
	ParticleDensities.resize(ParticlesCount);

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

			ParticleIndicies[i] = i;
		}
	}
	else
	{
		const glm::vec2 AvaliableSpace = ParentContainer->GetSize()*2.0f - glm::vec2(ParticleSize*2.0f);
		const float particleSpacing = 0.0f;
		const float spacing = ParticleSize*2.0f + particleSpacing;

		int32 particlesPerRow;
		int32 particlesPerCol;

		if (ParticlesCount * ParticleSize > ParentContainer->GetSize().y * 0.5f)
		{
			particlesPerRow = (int32)glm::sqrt(ParticlesCount * 2);
			particlesPerCol = (ParticlesCount - 1) / particlesPerRow + 1;
		}
		else
		{
			particlesPerRow = (int32)glm::sqrt(ParticlesCount);
			particlesPerCol = (ParticlesCount - 1) / particlesPerRow + 1;
		}

		
		for ( int32 i = 0; i < ParticlesCount; ++i )
		{
			float x = (i % particlesPerRow - particlesPerRow / 2.0f + 0.5f) * spacing;
			float y = (i / particlesPerRow - particlesPerCol / 2.0f + 0.5f) * spacing;
			Particles[i].Position = glm::vec2(x, y);
			OriginalPositions[i] = Particles[i].Position;

			ParticleIndicies[i] = i;
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

void FluidSimulation::OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime)
{
	const glm::ivec2 vSize = Engine->GetViewport()->GetViewportSize();
	const glm::vec2 rPos = (glm::vec2(CurrentPosition.x / vSize.x, CurrentPosition.y / vSize.y ) * 2.0f) - glm::vec2(1.0f);

	glm::vec2 halfWorldSize = WorldSize * 0.5f;

	const glm::vec2 mWorldPos = rPos * halfWorldSize;

	float Density = CalcDensity(mWorldPos);
	Log::LogMessage(ELog::Log, "density: {}", Density);
	

	std::pair<int32, int32> CellHash;
	if ( GetCellHash(GetCellCoord(mWorldPos), CellHash) )
		Log::LogMessage(ELog::Log, "cell coords: {}, Cell index: {}, {}...... {}", GetCellCoord(mWorldPos), CellHash.first, CellOccupied[CellHash.first] ? "occupied"  : "empty", CellHash.second);

	std::list<int32> HighlightParticles;
	GetRelatedParticles(mWorldPos, HighlightParticles);
	
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, HighlightParticles](int32 i)
		{
			bool h = std::find(HighlightParticles.begin(), HighlightParticles.end(), i) != HighlightParticles.end();
			Particles[i].Hightligh = h ? 1 : 0;
		});
	
}


///////////////////////////////////////////////

void FluidSimulation::Tick(float DeltaTime)
{
	if ( bPaused ) return;
	for ( int32 i = 0; i < 1; ++i )
		SimulationStep(DeltaTime / 4.0f);
}

glm::ivec2 FluidSimulation::GetCellCoord(const glm::vec2& Position) const
{
	glm::vec2 normilizedPos = Position + WorldSize * 0.5f;
	int32 cellX = static_cast<int32>(std::floor(normilizedPos.x / SmoothingRadius*0.5f));
    int32 cellY = static_cast<int32>(std::floor(normilizedPos.y / SmoothingRadius*0.5f));
	return glm::ivec2(cellX, cellY);
}

 bool FluidSimulation::GetCellHash(const glm::ivec2& Coords, std::pair<int32, int32>& OutHash) const
{
	if (Coords.x < 0 || Coords.y < 0 || Coords.x >= GridSize.x || Coords.y >= GridSize.y)
	{
		OutHash = std::pair<int32, int32>(-1,-1);
		return false;
	}
	int32 hash = Coords.y * GridSize.x + Coords.x;
	int32 cellIndex = hash % CellOccupied.size();
	int32 pIndex = hash % ParticleIndicies.size();

	OutHash = std::pair<int32, int32>(cellIndex, pIndex);
	return true;
}

void FluidSimulation::UpdateSpacialLookup()
{
	SpacialLookup.clear();
	SpacialLookup.resize(ParticleIndicies.size(), std::pair<int32, int32>(0, 0));

	StartIndices.clear();
	StartIndices.resize(ParticleIndicies.size(), -1);
	

	float cellSize = SmoothingRadius * 2.0f;
    GridSize.x = glm::ceil(WorldSize.x / cellSize);
    GridSize.y = glm::ceil(WorldSize.y / cellSize);
	
	CellOccupied.clear();
	CellOccupied.resize( GridSize.x * GridSize.y, false);

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this](int32 i)
		{
			std::pair<int32, int32> CellHash;
			if ( !GetCellHash(GetCellCoord(Particles[i].Position), CellHash) )
				return;
			CellOccupied[CellHash.first] = true;
			SpacialLookup[i] = std::pair<int32, int32>(CellHash.second, i);
		});

	std::sort(
		SpacialLookup.begin(),
		SpacialLookup.end(),
		[this](const std::pair<int32, int32>& left, const std::pair<int32, int32>& right)
		{	
			return left.first < right.first;
		});

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this](int32 lookupIndex)
		{
			int32 CellHash = SpacialLookup[lookupIndex].first;
			int32 prevCellHash = lookupIndex != 0 ? SpacialLookup[lookupIndex-1].first : -1;
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
	std::pair<int32, int32> CellHash;
	if ( !GetCellHash(CellCoords, CellHash) )
		return;

	if ( !CellOccupied[CellHash.first] )
		return;

	int32 CellKey = CellHash.second;

	if ( StartIndices.size() <= CellKey || CellKey < 0 )
		return;

	int32 StartIndex = StartIndices[CellKey];

	if ( StartIndex < 0 )
		return;
	
	for (  uint32 i = StartIndex; i < SpacialLookup.size(); ++i)
	{
		if ( SpacialLookup[i].first != CellKey )
			return;

		OutParticles.push_back(SpacialLookup[i].second);
	}
}

void FluidSimulation::SimulationStep(float DeltaTime)
{
	assert(ParticleDensities.size() == Particles.size() && ParticleDensities.size() == ParticleIndicies.size() );

	glm::vec2 gravityAcceleration = glm::vec2(0.0f, Gravity) * DeltaTime;

	// gravity and density
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, gravityAcceleration](int32 i)
		{
			Particles[i].Velocity += gravityAcceleration;
			ParticleDensities[i] = CalcDensity(Particles[i].Position);
		});

	// pressure
	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, DeltaTime](int32 i)
		{
			glm::vec2 preassureForce = CalcPressureForce(i);
			glm::vec2 preassureAcceleration = preassureForce;
			Particles[i].Velocity += preassureAcceleration * DeltaTime;
		});

	std::for_each(std::execution::par,
		ParticleIndicies.begin(),
		ParticleIndicies.end(),
		[this, DeltaTime](int32 i)
		{
			Particles[i].Position += Particles[i].Velocity * DeltaTime;
			glm::vec2 CollisionVelocity(0.0);
			if ( ParentContainer->CheckRoundCollision(Particles[i].Position, ParticleSize, CollisionVelocity) )
			{
				Particles[i].Velocity *= (CollisionVelocity * CollisionDamping);
			}
		});

	UpdateSpacialLookup();
}

float FluidSimulation::DensityToPeassure(float density) const
{
	float densityError = TargetDensity - density;
	float pressure = densityError * Preassure;
	return pressure;
}

glm::vec2 FluidSimulation::CalcPressureForce(int32 pIndex) const
{
	glm::vec2 pressure = glm::vec2(0.0f);

	glm::vec2 randDir = glm::normalize(glm::vec2((float)(rand()) / (float)(RAND_MAX), (float)(rand()) / (float)(RAND_MAX)));

	std::list<int32> RelatedParticles;
	GetRelatedParticles(Particles[pIndex].Position, RelatedParticles);
	for ( int32 otherIndex : RelatedParticles )
	{
		if ( pIndex == otherIndex ) continue;

		glm::vec2 offset = Particles[pIndex].Position - Particles[otherIndex].Position;
		float dst = glm::length(offset);

		glm::vec2 dir = dst != 0.0f ? offset / dst : randDir;
		float slope =  SmoothingKernelDerivative(dst, SmoothingRadius);
		float density = ParticleDensities[otherIndex];

		float sharedPressure = CalcSharedPressure(ParticleDensities[pIndex], ParticleDensities[otherIndex]);

		pressure += sharedPressure * dir * slope * ParticleMass / density;
	}
	return pressure;
}

float FluidSimulation::CalcSharedPressure(float densityA, float densityB) const
{
	float pressureA = DensityToPeassure(densityA);
	float pressureB = DensityToPeassure(densityB);
	return (pressureA + pressureB) / 2.0f;
}

float FluidSimulation::CalcDensity(const glm::vec2& samplePoint) const
{
	float density = 0.0f;

	std::list<int32> RelatedParticles;
	GetRelatedParticles(samplePoint, RelatedParticles);

	for ( int32& i : RelatedParticles )
	{
		float dst = glm::length(Particles[i].Position - samplePoint);
		float influence = SmoothingKernel(SmoothingRadius, dst);
		density += ParticleMass * influence;
	}
	return density;
}

float FluidSimulation::SmoothingKernel(float radius, float distance) const
{
	if ( distance >= radius ) return 0.0f;

	float volume = glm::pi<float>() * glm::pow(radius, 4) / 6.0f;
	return (radius - distance) * (radius - distance) / volume;
}

float FluidSimulation::SmoothingKernelDerivative(float radius, float distance) const
{
	if ( distance >= radius ) return 0.0f;
	
	float scale = 12 / (glm::pi<float>() * glm::pow(radius, 4));
	return (distance - radius) * scale;
}
