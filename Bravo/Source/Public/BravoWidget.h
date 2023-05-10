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

	// Position in normalized coords: [0..1]
	void SetPosition(const glm::vec2& _Position);
	const glm::vec2& GetPosition() const { return Position; }

	// Size in normalized coords: [0..1]
	void SetSize(const glm::vec2& _Size);
	const glm::vec2& GetSize() const { return Size; }

	// Origin in normalized coords: [0..1]
	void SetOrigin(const glm::vec2& _Origin);
	const glm::vec2& GetOrigin() const { return Origin; }

	// Returns actual widget size in pixels.
	// !! Might be different from intensional size if widget's content gets out of bounds. (text for example)
	virtual glm::vec2 GetActualSize() const;

	void Render();


protected:
	virtual bool Initialize_Internal() { return true; }
	virtual void Render_Internal() {}

	// Position in normalized coords: [0..1]
	glm::vec2 Position = glm::vec2(0.0f,0.0f);
	
	// Desired size in normalized coords: [0..1]
	glm::vec2 Size = glm::vec2(0.0f,0.0f);

	// Origin of the widget.
	glm::vec2 Origin = glm::vec2(0.0f, 0.0f);

	std::weak_ptr<class BravoHUD> HUD;
	std::shared_ptr<class BravoHUD> GetHUD() const { return HUD.expired() ? nullptr : HUD.lock(); }

	std::weak_ptr<class BravoAssetManager> AssetManager;
	std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager.expired() ? nullptr : AssetManager.lock(); }
};