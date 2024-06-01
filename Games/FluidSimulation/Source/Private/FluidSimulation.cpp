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

void FluidSimulation::SpawnParticles(int32 Count)
{
	const float particleSpacing = 2.5f;

	int32 particlesPerRow = (int32)glm::sqrt(Count);
	int32 particlesPerCol = (Count-1) / particlesPerRow + 1;
	float spacing = ParticleSize * 2.0f + particleSpacing;

	Particles.clear();
	Particles.resize(Count);
	for ( int32 i = 0; i < Count; ++i )
	{
		float x = (i % particlesPerRow - particlesPerRow / 2.0f + 0.5f) * spacing;
		float y = (i / particlesPerRow - particlesPerCol / 2.0f + 0.5f) * spacing;
		Particles[i].Position = glm::vec2(x, y);
	}
}

void FluidSimulation::ClearParticles()
{
	Particles.clear();
}

void FluidSimulation::Tick(float DeltaTime)
{
	for ( int32 i = 0; i < Particles.size(); ++i )
		UpdateParticle(i, DeltaTime);
}

void FluidSimulation::UpdateParticle(int32 i, float DeltaTime)
{
	Particle& p = Particles[i];
	
	glm::vec2 gravityAcceleration = glm::vec2(0.0f, -Gravity) * DeltaTime;

	p.Velocity = p.Velocity + ((p.Acceleration * DeltaTime) + gravityAcceleration);
	glm::vec2 vDir = glm::normalize(p.Velocity);
	float speed = glm::length(p.Velocity);
	float clampedSpeed = glm::fclamp(speed, 0.0f, MaxVelocity);
	p.Velocity = vDir * clampedSpeed;
	
	glm::vec2 CollisionVelocity(0.0);

	if ( ParentContainer->CheckRoundCollision(p.Position, ParticleSize, CollisionVelocity) )
	{
		p.Velocity *= (CollisionVelocity * CollisionDampling);
	}

	p.Position += p.Velocity * DeltaTime;
}
