#pragma once
#include "stdafx.h"
#include "BravoScreen.h"


class BravoScreen_Debug : public BravoScreen
{
public:
	template <typename... Args>
	BravoScreen_Debug(Args&&... args) :
		BravoScreen(std::forward<Args>(args)...)
	{}



protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;

private:
	ImFont* Font;
	const float UpdateRate = 1.0f;

	float TimeTillUpdate = 0.0f;

	std::string FPSStr;
	ImVec4 Color;
};