#pragma once
#include "stdafx.h"
#include "BravoObject.h"

struct BravoSelection
{
	glm::vec2 MousePosition = glm::vec2(-1.0f,-1.0f);
	std::shared_ptr<BravoObject> Object = nullptr;
	int32 InstanceIndex = 0;
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
