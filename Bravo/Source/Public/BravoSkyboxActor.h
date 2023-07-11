#pragma once
#include "BravoActor.h"
#include "BravoShader.h"
#include "BravoCubemap.h"
#include "BravoRenderable.h"

class BravoSkyboxActor : public BravoActor, public BravoRenderable
{
public:
	BravoSkyboxActor() :
		BravoActor(),
		BravoRenderable(INT_MAX-1)
	{
	}

	void SetCubemap(BravoCubemapPtr InCubemap);

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) override;
	virtual void OnDestroy() override;

private:
	BravoShaderPtr Shader;
	BravoCubemapPtr Cubemap;

	uint32 VAO = 0, VBO = 0;
};
