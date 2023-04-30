#pragma once

#include "BravoMesh.h"
#include "BravoEngine.h"
#include "BravoActor.h"
#include "BravoShader.h"


class BravoMeshActor : public BravoActor, public BravoTickable
{
public:
	void SetMesh(BravoMeshPtr InMesh);
	
protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnDestroy() override;
	virtual void Draw(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	virtual void DrawToShadowMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& Position) const override;

	BravoMeshPtr Mesh;
	BravoShaderPtr Shader;
};