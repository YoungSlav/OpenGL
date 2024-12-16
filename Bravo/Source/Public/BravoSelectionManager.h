#pragma once
#include "stdafx.h"
#include "BravoObject.h"

struct BravoSelection
{
	std::shared_ptr<class IBravoRenderable> Object = nullptr;
	int32 InstanceIndex = 0;
	bool operator==(const BravoSelection& rhs) const
	{
		return InstanceIndex == rhs.InstanceIndex && Object == rhs.Object;
	}
	bool operator!=(const BravoSelection& rhs) const
	{
		return InstanceIndex != rhs.InstanceIndex || Object != rhs.Object;
	}
};

class BravoSelectionManager : public BravoObject
{
public:
	template <typename... Args>
	BravoSelectionManager(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	const std::map<std::shared_ptr<class IBravoRenderable>, std::vector<int32>>& GetSelections() const { return ActiveSelections; }

	void SetAllowSelections(bool bAllow) { bAllowSelections = bAllow; }

protected:

	virtual bool Initialize_Internal() override;
	virtual void OnBeginPlay() override;
	virtual void OnDestroy() override;

	void OnViewportResized(const glm::ivec2& _Size);

	void OnMouseClicked(bool ButtonState, float DeltaTime);

	void ChangeSelection(const BravoSelection& Selection);
	void ClearSelections();

	void SpawnGizmo();
	void UpdateGizmo();
	void ClearGizmo();


private:

	std::map<std::shared_ptr<class IBravoRenderable>, std::vector<int32>> ActiveSelections;
	
	glm::ivec2 Size;
	std::shared_ptr<class BravoRenderTarget> SelectionRenderTarget;

	std::shared_ptr<class BravoGizmo> Gizmo = nullptr;

	bool bAllowSelections = false;
};
