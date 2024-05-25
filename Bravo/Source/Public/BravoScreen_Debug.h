#pragma once
#include "stdafx.h"
#include "BravoScreen.h"


class BravoScreen_Debug : public BravoScreen
{
public:

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;
	virtual void OnHUDResized(const glm::ivec2& NewSize) override;

private:

	ImFont* Font;
};