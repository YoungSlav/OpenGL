#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class PBRGameInstance : public BravoObject, public IBravoTickable
{
public:
	template <typename... Args>
	PBRGameInstance(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTickable()
	{}

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;

private:
	std::shared_ptr<class BravoCamera> Camera;
	std::shared_ptr<class BravoPlayer> Player;
	std::shared_ptr<class BravoSpotLightActor> SpotLight;
};