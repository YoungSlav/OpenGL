#pragma once
#include "BravoActor.h"
#include "BravoShaderAsset.h"
#include "BravoCubemapAsset.h"
#include "IBravoRenderable.h"

class BravoSkyboxActor : public BravoActor, public IBravoRenderable
{
public:
	BravoSkyboxActor() :
		BravoActor(),
		IBravoRenderable(ERenderPriority::Late)
	{
	}

	void SetCubemap(std::shared_ptr<BravoCubemapAsset> InCubemap);

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render() override;
	virtual void OnDestroy() override;

private:
	std::shared_ptr<BravoShaderAsset> Shader;
	std::shared_ptr<BravoCubemapAsset> Cubemap;

	uint32 VAO = 0, VBO = 0;
};
