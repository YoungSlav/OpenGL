#pragma once


#include "BravoActor.h"
#include "BravoShader.h"
#include "BravoMesh.h"

class BravoInfinitePlaneActor : public BravoActor
{
public:
	

protected:
	virtual void Init() override;
	virtual void Draw(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	virtual void OnDestroy() override;

	BravoShaderPtr Shader;
	BravoMeshPtr Mesh;
};
