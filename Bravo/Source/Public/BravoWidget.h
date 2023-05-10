#pragma once
#include "stdafx.h"
#include "SharedFromThis.h"

struct HUDVertex
{
	HUDVertex() = default;
	HUDVertex(const glm::vec2& _Position) :
		Position(_Position),
		TexCoords(glm::vec2(0.0f, 0.0f))
	{}
	HUDVertex(const glm::vec2& _Position, const glm::vec2& _TexCoords) :
		Position(_Position),
		TexCoords(_TexCoords)
	{}

	glm::vec2 Position = glm::vec2(0.0f, 0.0f);
	glm::vec2 TexCoords = glm::vec2(0.0f, 0.0f);
};

class BravoWidget : public SharedFromThis
{
public:
	BravoWidget(std::shared_ptr<class BravoHUD> _HUD, std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		HUD(_HUD),
		AssetManager(_AssetManager),
		SharedFromThis()
	{}

	bool Initialize();

	void SetPosition(const glm::ivec2& _Position);
	const glm::ivec2& GetPosition() const { return ScreenPosition; }

	void Render();
protected:
	virtual bool Initialize_Internal() { return true; }
	virtual void Render_Internal() {}

	glm::vec2 ScreenPosition = glm::vec2(0.0f,0.0f);

	std::weak_ptr<class BravoHUD> HUD;
	std::shared_ptr<class BravoHUD> GetHUD() const { return HUD.expired() ? nullptr : HUD.lock(); }

	std::weak_ptr<class BravoAssetManager> AssetManager;
	std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager.expired() ? nullptr : AssetManager.lock(); }
};