#pragma once
#include "stdafx.h"
#include "BravoWidget.h"


class BravoScreen : public BravoWidget
{
public:
	template <typename... Args>
	BravoScreen(Args&&... args) :
		BravoWidget(std::forward<Args>(args)...)
	{}


	int32 GetRenderPriority() const { return RenderPriority; }

protected:

	void SetRenderPriority(int32 _RenderPriority) { RenderPriority = _RenderPriority; }

	// Position in normalized coords: [0..1]
	void SetPosition(const glm::vec2& _Position) { Position = _Position; }
	const glm::vec2& GetPosition() const { return Position; }

	// Size in normalized coords: [0..1]
	void SetSize(const glm::vec2& _Size) { Size = _Size; }
	const glm::vec2& GetSize() const { return Size; }

	// Maxallowed size in normalized coords: [0..1]
	void SetMaxSize(const glm::vec2& _Size) { MaxSize = _Size; }
	const glm::vec2& GetMaxSize() const { return MaxSize; }

	// Origin in normalized coords: [0..1]
	void SetOrigin(const glm::vec2& _Origin) { Origin = _Origin; }
	const glm::vec2& GetOrigin() const { return Origin; }

	void SetTrueScaling(bool _bTrueScaling) { bTrueScaling = _bTrueScaling; }
	bool GetTrueScaling() const { return bTrueScaling; }

	void SetFontsScaling(bool _bScaleFonts) { bScaleFonts = _bScaleFonts; }
	bool GetFontsScaling() const { return bScaleFonts; }

	// Returns actual widget size and position in pixels.
	glm::vec4 GetBounds() const;


	virtual void OnDestroy() override;

	virtual void Render_Internal(float DeltaTime) override;

protected:
	// Position in normalized coords: [0..1]
	glm::vec2 Position = glm::vec2(0.0f,0.0f);
	
	// Desired size in normalized coords: [0..1]
	glm::vec2 Size = glm::vec2(0.0f,0.0f);

	// Origin of the screen.
	glm::vec2 Origin = glm::vec2(0.0f, 0.0f);

	// Max size in normalized coords: [0..1]
	glm::vec2 MaxSize = glm::vec2(0.0f,0.0f);

	// If true, the screen will be scaled to true HUD size along X. Otherwise only HUD height is scaling the screen.
	// !! Does not affect text
	bool bTrueScaling = false;

	bool bScaleFonts = false;
	float FontScaling = 1.0f;

private:
	int32 RenderPriority = 0;
};