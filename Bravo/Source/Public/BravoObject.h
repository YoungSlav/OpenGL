#pragma once

#include "stdafx.h"
#include "SharedFromThis.h"


class BravoEngine;

class BravoTickable : public SharedFromThis
{
	friend class BravoEngine;
protected:
	virtual void Tick(float DeltaTime) {};

private:
	void Update(float DeltaTime)
	{
		LifeTime += DeltaTime;
		Tick(DeltaTime);
	}

protected:
	float LifeTime = 0.0f;
};

class BravoObject : public SharedFromThis
{
	friend class BravoEngine;

public:
	BravoObject(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		SharedFromThis(),
		Engine(_Engine),
		Handle(_Handle)
	{}
	void Initialize();
	void Destroy();

	const BravoHandle& GetHandle() const { return Handle; }
	std::shared_ptr<BravoEngine> GetEngine() const
	{
		if ( Engine.expired() )
			return nullptr;
		return Engine.lock();
	}

protected:
	virtual void Initialize_Internal() {}
	virtual void OnDestroy() {}


private:
	std::weak_ptr<BravoEngine> Engine;

	const BravoHandle Handle = 0;
};