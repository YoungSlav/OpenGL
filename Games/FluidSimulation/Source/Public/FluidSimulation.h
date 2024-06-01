#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "IBravoRenderable.h"

struct Particle
{
	alignas(8) glm::vec2 Position = glm::vec2(0.0f);
	alignas(8) glm::vec2 Velocity = glm::vec2(0.0f);
	alignas(8) glm::vec2 Acceleration = glm::vec2(0.0f);
};

class FluidSimulation : public BravoObject, public IBravoTickable, public IBravoRenderable
{
public:
	template <typename... Args>
	FluidSimulation(std::shared_ptr<class FluidContainer> _ParentContainer, Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTickable(),
		IBravoRenderable(ERenderPriority::Early),
		ParentContainer(_ParentContainer)
	{}

	// SIMULATION PROPERTIES
	 
	int32 ParticleCount = 0;
	bool bRandomPositions = false;

	float ParticleMass = 1.0f;

	float ParticleSize = 5.0f; 

	float CollisionDamping = 0.3f;

	float Gravity = 9.8f;

	float MaxVelocity = 100.0f;

	// END SIMULATION PROPERTIES

	void SpawnParticles(int32 Count, bool bRandomPos);
	
	void Restart();
	void TogglePause() { bPaused = !bPaused; }
	bool IsPaused() const { return bPaused; }
		

private:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;

	void UpdateParticle(int32 index, float DeltaTime);

private:
	std::vector<Particle> Particles;
	std::shared_ptr<class FluidContainer> ParentContainer = nullptr;

	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint ParticlesSSBO = 0;
	int32 ParticlesCount = 0;

	std::shared_ptr<class BravoShaderAsset> Shader;

	bool bPaused = true;
	std::vector<glm::vec2> OriginalPositions;
};
