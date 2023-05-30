#pragma once
#include "stdafx.h"
#include "BravoScreen.h"


class BravoScreen_Debug : public BravoScreen, public BravoTickable
{
public:

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;


private:
	std::weak_ptr<class BravoWidget_Text> FPSWidget;

	float FPSUpdateRate = 0.5f;
	float FPSUpdateTimer = 0.0f;
};