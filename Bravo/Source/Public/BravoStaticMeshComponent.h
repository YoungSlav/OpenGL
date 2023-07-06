#pragma once

#include "BravoMesh.h"
#include "BravoEngine.h"
#include "BravoComponent.h"
#include "BravoShader.h"


class BravoStaticMeshComponent : public BravoComponent
{
public:

	void SetMesh(BravoMeshPtr InMesh);
	
protected:
	virtual bool Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& LightPosition) const override;

private:
	BravoMeshPtr Mesh;
	BravoShaderPtr Shader;
};