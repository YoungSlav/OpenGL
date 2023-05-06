#pragma once


#include "BravoActor.h"
#include "BravoShader.h"
#include "BravoMesh.h"

class BravoInfinitePlaneActor : public BravoActor
{
public:
	BravoInfinitePlaneActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoActor(_Engine, _Handle)
	{
		RenderPriority = INT_MAX;
	}

protected:
	virtual void Init() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	virtual void OnDestroy() override;

	BravoShaderPtr Shader;
	BravoMeshPtr Mesh;
};
