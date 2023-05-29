#pragma once
#include "stdafx.h"
#include "BravoScreen.h"


class BravoScreen_Debug : public BravoScreen, public BravoTickable
{
public:
	BravoScreen_Debug(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle, std::shared_ptr<BravoObject> _Owner) :
		BravoScreen(_Engine, _Handle, _Owner),
		BravoTickable()
	{}

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;


private:
	std::weak_ptr<class BravoWidget_Text> FPSWidget;

	float FPSUpdateRate = 0.5f;
	float FPSUpdateTimer = 0.0f;
};