#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "imgui.h"

class BravoWidget : public BravoObject
{
public:
	template <typename... Args>
	BravoWidget(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	void Render(float DeltaTime);


protected:

	virtual void OnHUDResized(const glm::ivec2& NewSize) {};

	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) = 0;

	std::shared_ptr<class BravoHUD> GetHUD() const { return HUD.expired() ? nullptr : HUD.lock(); }
	std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager.expired() ? nullptr : AssetManager.lock(); }

	const std::string& GetLabel() const { return Label; }

private:
	std::weak_ptr<class BravoHUD> HUD;
	std::weak_ptr<class BravoAssetManager> AssetManager;

	std::string Label;
};