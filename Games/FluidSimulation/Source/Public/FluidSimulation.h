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
		IBravoRenderable(ERenderPriority::Late),
		ParentContainer(_ParentContainer)
	{}

	// SIMULATION PROPERTIES

	float ParticleSize = 1.0f;

	float CollisionDampling = 0.5f;

	float Gravity = 9.8f;

	// END SIMULATION PROPERTIES

	void SpawnParticles(int32 Count);
	void ClearParticles();
	void UpdateScale(float _pScale) { Scale = _pScale; }
	

private:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;

	void UpdateParticle(int32 index, float DeltaTime);

private:
	std::vector<Particle> Particles;
	std::shared_ptr<class FluidContainer> ParentContainer;

	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint ParticlesSSBO = 0;
	int32 ParticlesCount = 0;

	float Scale = 1.0f;

	std::shared_ptr<class BravoShaderAsset> Shader;
};
