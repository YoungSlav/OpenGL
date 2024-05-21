#pragma once

#include "BravoStaticMeshAsset.h"
#include "BravoEngine.h"
#include "BravoComponent.h"
#include "IBravoRenderable.h"
#include "BravoShaderAsset.h"
#include "BravoMath.h"
#include "BravoTransform.h"
#include "ITransformable.h"


struct BravoInstanceData
{
	alignas(16) glm::mat4 Transform = glm::mat4(1.0f);
};

class BravoStaticMeshComponent : public BravoComponent, public IBravoRenderable
{
	class Instance : public ITransformable
	{
	public:
		Instance( const BravoInstanceData& Data, std::shared_ptr<ITransformable> _Parent, int32 _InstanceIndex) :
			ITransformable(BravoTransform(Data.Transform), _Parent),
			InstanceIndex(_InstanceIndex)
		{}
		~Instance() = default;

		void SetData(const BravoInstanceData& Data)
		{
			SetTransform(BravoTransform(Data.Transform));
		}
		BravoInstanceData GetData() const
		{
			BravoInstanceData data;
			data.Transform = GetTransform().GetTransformMatrix();
			return data;
		}

		int32 InstanceIndex = -1;
	};
public:
	BravoStaticMeshComponent() = default;
	BravoStaticMeshComponent(ERenderPriority _RenderPriority) : BravoComponent(), IBravoRenderable(_RenderPriority) {}
	BravoStaticMeshComponent(ERenderPriority _RenderPriority, ERenderGroup _RenderGroup) : BravoComponent(), IBravoRenderable(_RenderPriority, _RenderGroup) {}

	void SetMesh(std::shared_ptr<BravoStaticMeshAsset> InMesh);
	std::shared_ptr<class BravoMaterial> GetMaterial() const;
	void SetMaterial(std::shared_ptr<BravoMaterial> _Material);
	bool EnsureReady();

	int32 AddInstance(const BravoInstanceData& InstanceData);
	void UpdateInstance(int32 Index, const BravoInstanceData& NewInstance);
	void RemoveAllInstances();
	void RemoveInstances(int32 Index, int32 Count);
	BravoInstanceData GetInstanceData(int32 Index) const;

	inline int32 InstanceCount() const { return (int32)Instances.size(); }

	void UpdateInstanceBuffer();

protected:

	void OnInstanceTransformUpdated(const ITransformable* inst);

	virtual bool Initialize_Internal() override;
	virtual void Render() override;
	virtual void RenderSelectionID() override;
	virtual void RenderOutlineMask(int32 InstanceID) override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) override;
	virtual void OnDestroy() override;

private:
	std::vector<std::shared_ptr<Instance>> Instances;
	std::vector<BravoInstanceData> InstanceData;
	bool bInstanceStateDirty = true;

	GLuint VAO = 0;
	GLuint instancesUBO = 0;
	std::shared_ptr<BravoStaticMeshAsset> Mesh = nullptr;
	std::shared_ptr<class BravoMaterial> Material = nullptr;
	std::shared_ptr<class BravoShaderAsset> SelectionIDShader = nullptr;
	std::shared_ptr<class BravoShaderAsset> OutlineMaskShader = nullptr;
};
