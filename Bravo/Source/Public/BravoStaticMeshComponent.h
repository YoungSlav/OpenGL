#pragma once

#include "BravoStaticMeshAsset.h"
#include "BravoEngine.h"
#include "BravoComponent.h"
#include "IBravoRenderable.h"
#include "BravoShaderAsset.h"
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

class BravoStaticMeshComponent : public BravoComponent, public IBravoRenderable
{
public:
	void SetMesh(std::shared_ptr<BravoStaticMeshAsset> InMesh);
	std::shared_ptr<class BravoMaterial> GetMaterial() const;
	void SetMaterial(std::shared_ptr<BravoMaterial> _Material);
	bool EnsureReady();

	int32 AddInstance(const BravoMeshInstance& Instance, bool bUpdateInstanceBuffer = true);
	void UpdateInstance(int32 InstanceId, const BravoMeshInstance& NewInstance, bool bUpdateInstanceBuffer = true);
	void RemoveAllInstances(bool bUpdateInstanceBuffer = true);
	void RemoveInstances(int32 Index, int32 Count, bool bUpdateInstanceBuffer = true);
	int32 InstanceCount() const { return (int32)Instances.size(); }

	void UpdateInstanceBuffer();

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render() override;
	virtual void RenderSelection() override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) override;
	virtual void OnDestroy() override;

private:
	std::vector<BravoMeshInstance> Instances;
	bool bInstanceStateDirty = true;

	GLuint VAO = 0;
	GLuint instanceVBO = 0;
	std::shared_ptr<BravoStaticMeshAsset> Mesh = nullptr;
	std::shared_ptr<class BravoMaterial> Material = nullptr;
	std::shared_ptr<class BravoShaderAsset> SelectionShader = nullptr;
};
