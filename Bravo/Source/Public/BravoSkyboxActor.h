#pragma once
#include "BravoActor.h"
#include "BravoShader.h"
#include "BravoCubemap.h"

class BravoSkyboxActor : public BravoActor
{
public:
	BravoSkyboxActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoActor(_Engine, _Handle)
	{
		RenderPriority = INT_MAX-1;
	}

	void SetCubemap(BravoCubemapPtr InCubemap);

protected:
	virtual void Init() override;
	virtual void Draw(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	virtual void OnDestroy() override;

	BravoShaderPtr Shader;
	BravoCubemapPtr Cubemap;

	unsigned int VAO = 0, VBO = 0;
};
