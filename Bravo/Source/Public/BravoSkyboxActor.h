#pragma once
#include "BravoActor.h"
#include "BravoShader.h"
#include "BravoCubemap.h"

class BravoSkyboxActor : public BravoActor
{
public:
	BravoSkyboxActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle, std::shared_ptr<BravoObject> _Owner) :
		BravoActor(_Engine, _Handle, _Owner)
	{
		RenderPriority = INT_MAX-1;
	}

	void SetCubemap(BravoCubemapPtr InCubemap);

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	virtual void OnDestroy() override;

private:
	BravoShaderPtr Shader;
	BravoCubemapPtr Cubemap;

	uint32 VAO = 0, VBO = 0;
};
