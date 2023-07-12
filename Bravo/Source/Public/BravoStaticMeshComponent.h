#pragma once

#include "BravoMesh.h"
#include "BravoEngine.h"
#include "BravoComponent.h"
#include "BravoRenderable.h"
#include "BravoShader.h"
#include "BravoMath.h"

class BravoMeshInstance
{
public:
	glm::mat4 TransfromMatrix;

	BravoMeshInstance()
	{
		UpdateTransformMatrix();
	}
	
	BravoMeshInstance(const glm::vec3& _Location, const glm::vec3& _Rotation, const glm::vec3& _Scale) :
		Location(_Location),
		Rotation(_Rotation),
		Scale(_Scale)
	{
		UpdateTransformMatrix();
	}

private:

	void UpdateTransformMatrix()
	{
		glm::mat4 transform;
		transform = glm::translate(transform, Location);
		transform = transform * glm::toMat4(BravoMath::EulerToQuat(Rotation));
		transform = glm::scale(transform, Scale);
		TransfromMatrix = transform;
	}

	glm::vec3 Location = glm::vec3(0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f);
	glm::vec3 Scale = glm::vec3(1.0f);
};



class BravoStaticMeshComponent : public BravoComponent, public BravoRenderable
{
public:
	void SetMesh(BravoMeshPtr InMesh);
	BravoMaterialPtr GetMaterial() const;
	void SetMaterial(BravoMaterialPtr _Material);
	bool EnsureReady();

	int32 AddInstance(const BravoMeshInstance& Instance, bool bUpdateInstanceBuffer = true);
	void RemoveInstances(int32 Index, int32 Count, bool bUpdateInstanceBuffer = true);
	void UpdateInstanceBuffer();

	int32 TotalInstanceCount() const { return (int32)Instances.size(); }

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShader> Shader) override;
	virtual void OnDestroy() override;

private:
	std::vector<BravoMeshInstance> Instances;

	GLuint VAO = 0;
	GLuint instanceVBO = 0;
	BravoMeshPtr Mesh;
	BravoShaderPtr Shader;
	BravoMaterialPtr Material;
};
