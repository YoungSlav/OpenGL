#pragma once

#include "BravoMesh.h"
#include "BravoEngine.h"
#include "BravoActor.h"
#include "BravoShader.h"


class BravoMeshActor : public BravoActor, public BravoTickable
{
public:
	BravoMeshActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoActor(_Engine, _Handle),
		BravoTickable()
	{}

	void SetMesh(BravoMeshPtr InMesh);
	
protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnDestroy() override;
	virtual void Draw(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	virtual void DrawToShadowMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& LightPosition) const override;

	BravoMeshPtr Mesh;
	BravoShaderPtr Shader;
};