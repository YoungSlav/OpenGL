#pragma once

#include "BravoMesh.h"
#include "BravoEngine.h"
#include "BravoComponent.h"
#include "BravoRenderable.h"
#include "BravoShader.h"

class BravoStaticMeshComponent : public BravoComponent, public BravoRenderable
{
public:
	void SetMesh(BravoMeshPtr InMesh);
	BravoMaterialPtr GetMaterial() const;
	void SetMaterial(BravoMaterialPtr _Material);
	bool EnsureReady();

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShader> Shader) override;
	virtual void OnDestroy() override;

private:
	GLuint VAO = 0;
	BravoMeshPtr Mesh;
	BravoShaderPtr Shader;
	BravoMaterialPtr Material;
};
