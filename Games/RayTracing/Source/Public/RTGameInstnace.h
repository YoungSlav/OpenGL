#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class RTGameInstnace : public BravoObject, public IBravoTickable
{
public:
	template <typename... Args>
	RTGameInstnace(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTickable()
	{}

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;


	std::shared_ptr<class BravoDirectionalLightActor> DirLight;
	std::shared_ptr<class BravoCamera> Camera;
	std::shared_ptr<class BravoPlayer> Player;
	std::vector<std::shared_ptr<class BravoActor>> ShowRooms;

	std::shared_ptr<class BravoPointLightActor> PointLightActor;
	std::shared_ptr<class BravoSpotLightActor> SpotLightActor;
};