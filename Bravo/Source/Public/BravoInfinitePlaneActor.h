#pragma once


#include "BravoActor.h"
#include "IBravoRenderable.h"
#include "BravoShaderAsset.h"
#include "BravoStaticMeshAsset.h"

class BravoInfinitePlaneActor : public BravoActor, public IBravoRenderable
{
public:
	BravoInfinitePlaneActor() :
		BravoActor(),
		IBravoRenderable(INT_MAX)
	{
	}

	bool EnsureReady();

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render() override;
	virtual void OnDestroy() override;

private:
	std::shared_ptr<BravoShaderAsset> Shader;
	std::shared_ptr<BravoStaticMeshAsset> Mesh;

	GLuint VAO = 0;
};
