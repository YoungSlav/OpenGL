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

typedef MulticastDelegate<const BravoSelection&> OnSelectionSignature;

class BravoSelectionManager : public BravoObject
{
public:
	OnSelectionSignature OnObjectSelected;

protected:

	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

	void OnViewportResized(const glm::ivec2& _Size);

	void OnMouseClicked(bool ButtonState, float DeltaTime);

private:
	glm::ivec2 Size;
	std::shared_ptr<class BravoRenderTarget> SelectionRenderTarget;
};
