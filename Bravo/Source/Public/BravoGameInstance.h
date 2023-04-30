#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class BravoGameInstance : public BravoObject, public BravoTickable
{
protected:
	virtual void Init() override;
	virtual void OnDestroy() override;
	virtual void Tick(float DeltaTime) override;

	BravoEngine* Engine;
	std::vector<class BravoActor*> Children;

	
	class BravoDirLightActor* dirLightActor;

	class BravoSpotLightActor* spotLights[2];

};