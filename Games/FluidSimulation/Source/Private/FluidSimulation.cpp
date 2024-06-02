#include "FluidSimulation.h"
#include "FluidContainer.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoCamera.h"

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


	SpawnParticles(ParticleCount, true);
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
	if ( ParticlesCount != Particles.size() )
	{
		ParticlesCount = Particles.size();
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


void FluidSimulation::SpawnParticles(int32 Count, bool bRandomPos)
{
	assert( ParentContainer != nullptr );
	ParticleCount = Count;
	bPaused = true;
	bHasStarted = false;
	bRandomPositions = bRandomPos;

	Particles.clear();
	Particles.resize(Count);
	
	OriginalPositions.clear();
	OriginalPositions.resize(Count);
	
	ParticleIndicies.clear();
	ParticleIndicies.resize(Count);

	ParticleDensities.clear();
	ParticleDensities.resize(Count);

	if ( Count == 0 )
		return;

	ParticleSize = ParticleSize;


	if ( bRandomPos )
	{
		for ( int32 i = 0; i < Count; ++i )
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

		if (Count * ParticleSize > ParentContainer->GetSize().y * 0.5f)
		{
			particlesPerRow = (int32)glm::sqrt(Count * 2);
			particlesPerCol = (Count - 1) / particlesPerRow + 1;
		}
		else
		{
			particlesPerRow = (int32)glm::sqrt(Count);
			particlesPerCol = (Count - 1) / particlesPerRow + 1;
		}

		
		for ( int32 i = 0; i < Count; ++i )
		{
			float x = (i % particlesPerRow - particlesPerRow / 2.0f + 0.5f) * spacing;
			float y = (i / particlesPerRow - particlesPerCol / 2.0f + 0.5f) * spacing;
			Particles[i].Position = glm::vec2(x, y);
			OriginalPositions[i] = Particles[i].Position;

			ParticleIndicies[i] = i;
		}
	}
}

void FluidSimulation::Reset()
{
	assert( OriginalPositions.size() == Particles.size() );
	for ( int32 i = 0; i < OriginalPositions.size(); ++i )
	{
		Particles[i] = Particle();
		Particles[i].Position = OriginalPositions[i];
	}
	bHasStarted = false;
	bPaused = true;
}

void FluidSimulation::TogglePause()
{
	bPaused = !bPaused;
	if ( !bPaused )
		bHasStarted = true;
}


///////////////////////////////////////////////

void FluidSimulation::Tick(float DeltaTime)
{
	if ( bPaused ) return;
	for ( int32 i = 0; i < 4; ++i )
		SimulationStep(DeltaTime / 4.0f);
}

void FluidSimulation::SimulationStep(float DeltaTime)
{
	assert(ParticleDensities.size() == Particles.size() && ParticleDensities.size() == ParticleIndicies.size() );

	glm::vec2 gravityAcceleration = glm::vec2(0.0f, -Gravity) * DeltaTime;


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
			glm::vec2 preassureAcceleration = preassureForce / ParticleDensities[i];
			Particles[i].Velocity = preassureAcceleration * DeltaTime;
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

	for ( int32 i = 0; i < Particles.size(); ++i )
	{
		if ( pIndex == i ) continue;

		glm::vec2 offset = Particles[pIndex].Position - Particles[i].Position;
		float dst = glm::length(offset);

		glm::vec2 dir = dst != 0.0f ? offset / dst : randDir;
		float slope =  SmoothingKernelDerivative(dst, SmoothingRadius);
		float density = ParticleDensities[i];

		float sharedPressure = CalcSharedPressure(ParticleDensities[pIndex], ParticleDensities[i]);

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

	for ( int32 i = 0; i < Particles.size(); ++i )
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
