#pragma once
#include "stdafx.h"
#include "BravoObject.h"


struct BravoSelection
{
	std::shared_ptr<class BravoObject> Object = nullptr;
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

typedef MulticastDelegate<> OnSelectionChangedSignature;

class BravoSelectionManager : public BravoObject
{
	struct WeakPtrCompareSelections
	{
		bool operator()(const std::weak_ptr<class BravoObject>& a, const std::weak_ptr<class BravoObject>& b) const
		{
			return a.lock().get() < b.lock().get();
		}
	};

	struct WeakPtrCompareHighlights
	{
		bool operator()(const std::weak_ptr<class IBravoRenderable>& a, const std::weak_ptr<class IBravoRenderable>& b) const
		{
			return a.lock().get() < b.lock().get();
		}
	};

public:
	template <typename... Args>
	BravoSelectionManager(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	const std::map<std::weak_ptr<class BravoObject>, std::vector<int32>, WeakPtrCompareSelections>& GetActiveSelections() const { return ActiveSelections; }
	const std::map<std::weak_ptr<class IBravoRenderable>, std::vector<int32>, WeakPtrCompareHighlights>& GetActiveHighlights() const { return ActiveHighlights; }

	void SetAllowSelections(bool bAllow) { bAllowSelections = bAllow; }

	void SelectObject(std::shared_ptr<BravoObject> obj, bool bAddToSelection = false);
	void ClearSelections(bool bBroadcastChange = true);

	OnSelectionChangedSignature OnSelectionChanged;

protected:

	virtual bool Initialize_Internal() override;
	virtual void OnBeginPlay() override;
	virtual void OnDestroy() override;

	void OnViewportResized(const glm::ivec2& _Size);

	void OnMouseClicked(bool ButtonState, float DeltaTime);
	
	void DispatchSelection(BravoHandle Handle, int32 Instance, bool bSelectIndividualInstance, bool bAddToSelection, bool bSelectComponent);

	void AddToSelection(const BravoSelection& Selection, bool bSelectIndividualInstance);
	void UpdateHighlights();
	
	void NormalizeSelections();

	void SpawnGizmo();
	void UpdateGizmo();
	void HideGizmo();


private:

	

	std::map<std::weak_ptr<IBravoRenderable>, std::vector<int32>, WeakPtrCompareHighlights> ActiveHighlights;
	std::map<std::weak_ptr<class BravoObject>, std::vector<int32>, WeakPtrCompareSelections> ActiveSelections;
	
	glm::ivec2 Size;
	std::shared_ptr<class BravoRenderTarget> SelectionRenderTarget;

	std::shared_ptr<class BravoGizmo> Gizmo = nullptr;

	bool bAllowSelections = true;
};
