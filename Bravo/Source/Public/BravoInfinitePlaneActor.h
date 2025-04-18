#pragma once


#include "BravoActor.h"
#include "IBravoRenderable.h"
#include "BravoShaderAsset.h"
#include "BravoStaticMeshAsset.h"

class BravoInfinitePlaneActor : public BravoActor, public IBravoRenderable
{
public:
	template <typename... Args>
	BravoInfinitePlaneActor(Args&&... args) :
		BravoActor(std::forward<Args>(args)...),
		IBravoRenderable(ERenderPriority::Late)
	{}



	bool EnsureReady();

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render() override;
	virtual void OnDestroy() override;

private:
	std::shared_ptr<BravoRenderShaderAsset> Shader;
	std::shared_ptr<BravoStaticMeshAsset> Mesh;
};
