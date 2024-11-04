#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class BravoGameInstance : public BravoObject, public IBravoTickable
{
public:
	template <typename... Args>
	BravoGameInstance(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTickable()
	{}

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;

	void TestTransforms();

	void SpawnDirLights();
	
	

	std::shared_ptr<class BravoCamera> Camera;
	std::shared_ptr<class BravoPlayer> Player;
};