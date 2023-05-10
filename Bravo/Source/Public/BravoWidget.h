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
	BravoWidget(std::shared_ptr<class BravoHUD> _HUD) : 
		HUD(_HUD),
		SharedFromThis()
	{}

	void Render();

	void SetPosition(const glm::ivec2& _Position);
	const glm::ivec2& GetPosition() const { return ScreenPosition; }

protected:
	virtual void Render_Internal() {}

	glm::vec2 ScreenPosition = glm::vec2(0.0f,0.0f);

	std::weak_ptr<class BravoHUD> HUD;
	std::shared_ptr<class BravoHUD> GetHUD() const { return HUD.expired() ? nullptr : HUD.lock(); }
};