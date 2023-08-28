#pragma once


#include "BravoActor.h"
#include "BravoRenderable.h"
#include "BravoShaderAsset.h"
#include "BravoStaticMeshAsset.h"

class BravoInfinitePlaneActor : public BravoActor, public BravoRenderable
{
public:
	BravoInfinitePlaneActor() :
		BravoActor(),
		BravoRenderable(INT_MAX)
	{
	}

	bool EnsureReady();

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) override;
	virtual void OnDestroy() override;

private:
	std::shared_ptr<BravoShaderAsset> Shader;
	std::shared_ptr<BravoStaticMeshAsset> Mesh;

	GLuint VAO = 0;
};
