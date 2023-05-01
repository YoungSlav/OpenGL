#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class BravoGameInstance : public BravoObject, public BravoTickable
{
public:
	BravoGameInstance(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoObject(_Engine, _Handle),
		BravoTickable()
	{}

protected:
	virtual void Init() override;
	virtual void OnDestroy() override;
	virtual void Tick(float DeltaTime) override;

	
	std::vector<std::weak_ptr<class BravoActor>> Children;

	std::weak_ptr<class BravoDirLightActor> dirLightActor;

	std::weak_ptr<class BravoSpotLightActor> spotLights[2];

};