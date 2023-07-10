#pragma once


#include "BravoActor.h"
#include "BravoRenderable.h"
#include "BravoShader.h"
#include "BravoMesh.h"

class BravoInfinitePlaneActor : public BravoActor, public BravoRenderable
{
public:
	BravoInfinitePlaneActor() :
		BravoActor(),
		BravoRenderable(INT_MAX)
	{
	}

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	virtual void OnDestroy() override;

	BravoShaderPtr Shader;
	BravoMeshPtr Mesh;
};
