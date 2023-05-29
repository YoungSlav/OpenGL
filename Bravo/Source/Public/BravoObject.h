#pragma once

#include "stdafx.h"
#include "SharedFromThis.h"


class BravoEngine;

class BravoTickable
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
public:
	BravoObject(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle, std::shared_ptr<BravoObject> _Owner) :
		SharedFromThis(),
		Engine(_Engine),
		Handle(_Handle),
		Owner(_Owner)
	{}
	bool Initialize();
	void Destroy();

	const BravoHandle& GetHandle() const { return Handle; }

protected:
	virtual bool Initialize_Internal() { return true; }
	virtual void OnDestroy() {}


protected:
	const std::weak_ptr<BravoObject> Owner;
	const std::shared_ptr<BravoEngine> Engine;

private:
	const BravoHandle Handle = 0;
};