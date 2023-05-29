#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class BravoScreen : public BravoObject
{
public:
	BravoScreen(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoObject(_Engine, _Handle)
	{}

	int32 GetRenderPriority() const { return RenderPriority; }
	void Render();

	void AddWidget(std::shared_ptr<class BravoWidget> _Widget);
	void RemoveWidget(std::shared_ptr<class BravoWidget> _Widget);
	void SetRenderPriority(int32 _RenderPriority);

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

	std::shared_ptr<class BravoHUD> GetHUD() const { return HUD.expired() ? nullptr : HUD.lock(); }

	std::weak_ptr<class BravoAssetManager> AssetManager;
	std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager.expired() ? nullptr : AssetManager.lock(); }

private:
	std::vector<std::shared_ptr<class BravoWidget>> Widgets;
	std::weak_ptr<class BravoHUD> HUD;

	int32 RenderPriority = 0;
};