#pragma once
#include "stdafx.h"

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

class BravoObject
{
	friend class BravoEngine;

public:	
	void Destroy();
	BravoObjectHandle GetHandle() { return Handle; }

protected:
	virtual void Init() {}
	virtual void OnDestroy() {}

public:
	virtual ~BravoObject() {};

private:
	void PreInit(class BravoEngine* InEngine, BravoObjectHandle InHandle);

protected:
	// pointer to Engine singleton
	class BravoEngine* Engine = nullptr;

private:
	// handle of this object in Engine
	BravoObjectHandle Handle;
};