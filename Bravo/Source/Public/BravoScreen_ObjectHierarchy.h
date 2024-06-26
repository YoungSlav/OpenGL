#pragma once

#include "stdafx.h"
#include "BravoScreen.h"

class BravoScreen_ObjectHierarchy : public BravoScreen
{
public:
	template <typename... Args>
	BravoScreen_ObjectHierarchy(std::shared_ptr<class BravoObject> _RootObject, Args&&... args) :
		BravoScreen(std::forward<Args>(args)...),
		RootObject(_RootObject)
	{}



protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;

private:
	void RenderNode_Recursive(std::shared_ptr<class BravoObject> obj, int32 Depth);
	
	void OnToggleHUD(bool ButtonState, float DeltaTime);

private:
	std::shared_ptr<class BravoObject> RootObject;

	
	bool bShowHUD = false;
};

