#pragma once

#include "BravoStaticMeshAsset.h"
#include "BravoEngine.h"
#include "BravoComponent.h"
#include "IBravoRenderable.h"
#include "BravoShaderAsset.h"
#include "BravoMath.h"
#include "BravoTransform.h"
#include "IBravoTransformable.h"


struct BravoInstanceData
{
	BravoInstanceData() = default;
	
	BravoInstanceData(const BravoTransform& transform) :
		Transform(transform.GetTransformMatrix()) {}
	
	alignas(16) glm::mat4 Transform = glm::mat4(1.0f);
};

class BravoStaticMeshInstance : public IBravoTransformable
{
public:
	BravoStaticMeshInstance( const BravoInstanceData& data, std::shared_ptr<IBravoTransformable> _Parent, int32 _InstanceIndex) :
		IBravoTransformable(BravoTransform(data.Transform), _Parent),
		InstanceIndex(_InstanceIndex),
		Data(data)
	{}
	~BravoStaticMeshInstance() = default;

	void SetData(const BravoInstanceData& data)
	{
		Data = data;
		if ( !BravoMath::IsNearlyEqual(data.Transform, GetTransform()) )
			SetTransform(BravoTransform(data.Transform));
	}
	const BravoInstanceData& GetData() const
	{
		Data.Transform = GetTransform().GetTransformMatrix();
		return Data;
	}

	int32 InstanceIndex = -1;
	
private:
	mutable BravoInstanceData Data;
};

class BravoStaticMeshComponent : public BravoComponent, public IBravoRenderable
{

public:
	
	template <typename... Args>
	BravoStaticMeshComponent(Args&&... args) :
		BravoComponent(std::forward<Args>(args)...),
		IBravoRenderable()
	{}

	template <typename... Args>
	BravoStaticMeshComponent(ERenderPriority _RenderPriority, Args&&... args) :
		BravoComponent(std::forward<Args>(args)...),
		IBravoRenderable(_RenderPriority)
	{}

	template <typename... Args>
	BravoStaticMeshComponent(ERenderPriority _RenderPriority, ERenderGroup _RenderGroup, Args&&... args) :
		BravoComponent(std::forward<Args>(args)...),
		IBravoRenderable(_RenderPriority, _RenderGroup)
	{}

	void SetMesh(std::shared_ptr<BravoStaticMeshAsset> InMesh);
	std::shared_ptr<class BravoMaterial> GetMaterial() const;
	void SetMaterial(std::shared_ptr<BravoMaterial> _Material);
	bool EnsureReady();

	int32 AddInstance(const BravoInstanceData& InstanceData);
	void UpdateInstance(int32 Index, const BravoInstanceData& NewInstance);
	void RemoveAllInstances();
	void RemoveInstances(int32 Index, int32 Count);
	std::shared_ptr<BravoStaticMeshComponent::BravoStaticMeshInstance> GetInstance(int32 Index) const;

	inline int32 InstanceCount() const { return (int32)Instances.size(); }

	void UpdateInstanceBuffer();

	virtual void ClearSelection() override;
	virtual void SetSelection(const std::vector<int32>& SelectedInstances) override;
	virtual void RenderSelectionID() override;
	virtual void RenderOutlineMask() override;

protected:
	void OnInstanceTransformUpdated(const IBravoTransformable* inst);

	virtual bool Initialize_Internal() override;
	virtual void Render() override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) override;
	virtual void OnDestroy() override;

private:
	std::vector<std::shared_ptr<BravoStaticMeshInstance>> Instances;
	std::vector<BravoInstanceData> InstanceData;
	bool bInstanceStateDirty = true;

	GLuint InstancesSSBO = 0;
	
	GLuint SelectedInstancesSSBO = 0;
	std::vector<int32> SelectedInstances;
	int32 SelectedInstancesCount = 0;


	std::shared_ptr<BravoStaticMeshAsset> Mesh = nullptr;
	std::shared_ptr<class BravoMaterial> Material = nullptr;
	std::shared_ptr<class BravoShaderAsset> SelectionIDShader = nullptr;
	std::shared_ptr<class BravoShaderAsset> OutlineMaskShader = nullptr;
};
