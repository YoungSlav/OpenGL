#pragma once
#include "stdafx.h"

#include "BravoObject.h"
#include "BravoActor.h"
#include "BravoViewport.h"

class BravoViewport;
class BravoInput;

class BravoEngine
{
	friend class BravoObject;
	static BravoEngine* Instance;

public:
	static BravoEngine* GetInstance();
	BravoEngine();
	~BravoEngine();

	void StartGame();
	void Tick(float DeltaTime);
	void StopGame();
	
	template <typename Class>
	Class* SpawnObject()
	{
		Class* newObject = new Class();
		if ( BravoObject* asBravoObject = dynamic_cast<BravoObject*>(newObject) )
		{
			asBravoObject->PreInit(this, LastUsedHandle);
			Objects.insert( std::pair<BravoObjectHandle, BravoObject*>(LastUsedHandle, asBravoObject) );

			if ( BravoTickable* AsTickable = dynamic_cast<BravoTickable*>(asBravoObject) )
			{
				TickableObjects.insert( std::pair<BravoObjectHandle, BravoTickable*>(LastUsedHandle, AsTickable)	);
			}
			if ( Viewport )
			{
				if ( BravoActor* asActor = dynamic_cast<BravoActor*>(asBravoObject) )
				{
					Viewport->AddActor(asActor);
				}
			}

			++LastUsedHandle;

			asBravoObject->Init();
			return newObject;
		}
		delete newObject;
		return nullptr;
	}

	BravoViewport* const GetViewport() { return Viewport; }
	BravoInput* const GetInput() { return Input; }
	

private:
	void DestroyObject(BravoObjectHandle InHandle);

private:
	std::map< BravoObjectHandle, BravoObject* > Objects;
	std::map< BravoObjectHandle, BravoTickable* > TickableObjects;
	BravoObjectHandle LastUsedHandle = 0;
	

	BravoViewport* Viewport = nullptr;
	BravoInput* Input = nullptr;

private:
	// don't allow to copy!
	BravoEngine(const BravoEngine& other) = delete;
};
