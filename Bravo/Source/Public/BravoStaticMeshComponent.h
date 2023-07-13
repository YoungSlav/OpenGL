#pragma once

#include "BravoMesh.h"
#include "BravoEngine.h"
#include "BravoComponent.h"
#include "BravoRenderable.h"
#include "BravoShader.h"
#include "BravoMath.h"
#include "BravoTransform.h"

class BravoMeshInstance
{
public:
	glm::mat4 TransfromMatrix;
		
	BravoMeshInstance(const BravoTransform& _Transform)
	{
		TransfromMatrix = _Transform.GetTransformMatrix();
	}
};



class BravoStaticMeshComponent : public BravoComponent, public BravoRenderable
{
public:
	void SetMesh(BravoMeshPtr InMesh);
	BravoMaterialPtr GetMaterial() const;
	void SetMaterial(BravoMaterialPtr _Material);
	bool EnsureReady();

	int32 AddInstance(const BravoMeshInstance& Instance, bool bUpdateInstanceBuffer = true);
	void UpdateInstance(int32 InstanceId, const BravoMeshInstance& NewInstance, bool bUpdateInstanceBuffer = true);
	void RemoveAllInstances(bool bUpdateInstanceBuffer = true);
	void RemoveInstances(int32 Index, int32 Count, bool bUpdateInstanceBuffer = true);
	int32 InstanceCount() const { return Instances.size(); }
	void UpdateInstanceBuffer();

	int32 TotalInstanceCount() const { return (int32)Instances.size(); }

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShader> Shader) override;
	virtual void OnDestroy() override;

private:
	std::vector<BravoMeshInstance> Instances;
	bool bInstanceStateDirty = true;

	GLuint VAO = 0;
	GLuint instanceVBO = 0;
	BravoMeshPtr Mesh;
	BravoShaderPtr Shader;
	BravoMaterialPtr Material;
};
