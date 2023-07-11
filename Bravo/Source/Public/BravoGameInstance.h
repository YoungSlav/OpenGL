#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class BravoGameInstance : public BravoObject, public BravoTickable
{
public:

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;

	void SpawnCubes();
	void SpawnTest();

	std::vector<std::weak_ptr<class BravoSpotLightActor>> spotLights;
	std::vector<std::weak_ptr<class BravoActor>> Cubes;
};