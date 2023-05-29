#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class BravoGameInstance : public BravoObject, public BravoTickable
{
public:
	BravoGameInstance(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle, std::shared_ptr<BravoObject> _Owner) :
		BravoObject(_Engine, _Handle, _Owner),
		BravoTickable()
	{}

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;

	std::vector<std::weak_ptr<class BravoSpotLightActor>> spotLights;
};