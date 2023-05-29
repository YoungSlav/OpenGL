#pragma once
#include "stdafx.h"
#include "BravoObject.h"

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

class BravoWidget : public BravoObject
{
public:
	BravoWidget(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) : 
		BravoObject(_Engine, _Handle)
	{}

	void SetOwnerScreen(std::shared_ptr<class BravoScreen> _OwnerScreen);
	std::shared_ptr<class BravoScreen> GetOwnerScreen() const { return OwnerScreen.expired() ? nullptr : OwnerScreen.lock(); }

	// Position in normalized coords: [0..1]
	void SetPosition(const glm::vec2& _Position);
	const glm::vec2& GetPosition() const { return Position; }

	// Size in normalized coords: [0..1]
	void SetSize(const glm::vec2& _Size);
	const glm::vec2& GetSize() const { return Size; }

	// Origin in normalized coords: [0..1]
	void SetOrigin(const glm::vec2& _Origin);
	const glm::vec2& GetOrigin() const { return Origin; }

	void SetTrueScaling(bool _bTrueScaling);
	bool GetTrueScaling() const { return bTrueScaling; }

	// Returns actual widget size in pixels.
	// !! Might be different from intensional size if widget's content gets out of bounds. (text for example)
	virtual glm::vec2 GetActualSize() const;

	void Render();


protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal() = 0;
	virtual void OnDestroy() override;

	std::shared_ptr<class BravoHUD> GetHUD() const { return HUD.expired() ? nullptr : HUD.lock(); }
	std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager.expired() ? nullptr : AssetManager.lock(); }

private:
	// Position in normalized coords: [0..1]
	glm::vec2 Position = glm::vec2(0.0f,0.0f);
	
	// Desired size in normalized coords: [0..1]
	glm::vec2 Size = glm::vec2(0.0f,0.0f);

	// Origin of the widget.
	glm::vec2 Origin = glm::vec2(0.0f, 0.0f);

	// If true, the widget will be scaled to true HUD size along X. Otherwise only HUD height is scaling the widget.
	// !! Does not affect text
	bool bTrueScaling = false;

	std::weak_ptr<class BravoScreen> OwnerScreen;
	std::weak_ptr<class BravoHUD> HUD;
	std::weak_ptr<class BravoAssetManager> AssetManager;
};