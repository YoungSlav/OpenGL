#pragma once
#include "stdafx.h"
#include "BravoScreen.h"


class BravoScreen_Debug : public BravoScreen
{
public:

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;

private:
	ImFont* Font;
};