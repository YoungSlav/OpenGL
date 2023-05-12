#pragma once
#include "stdafx.h"
#include "SharedFromThis.h"

class BravoScreen : public SharedFromThis
{
public:
	BravoScreen(std::shared_ptr<class BravoHUD> _HUD, std::shared_ptr<class BravoAssetManager> _AssetManager) :
		HUD(_HUD),
		AssetManager(_AssetManager)
	{}

	bool Initialize();
	int32 GetRenderPriority() const { return RenderPriority; }
	void Render();

protected:
	virtual bool Initialize_Internal() { return true; }


protected:
	std::weak_ptr<class BravoHUD> HUD;
	std::shared_ptr<class BravoHUD> GetHUD() const { return HUD.expired() ? nullptr : HUD.lock(); }

	std::weak_ptr<class BravoAssetManager> AssetManager;
	std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager.expired() ? nullptr : AssetManager.lock(); }

	std::vector<std::shared_ptr<class BravoWidget>> Widgets;

	int32 RenderPriority = 0;
};