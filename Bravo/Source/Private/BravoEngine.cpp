#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoInput.h"


BravoEngine* BravoEngine::Instance = nullptr;

BravoEngine* BravoEngine::GetInstance()
{
	return Instance;
}

BravoEngine::BravoEngine()
{
	BravoEngine::Instance = this;
	Input = SpawnObject<BravoInput>();
	Viewport = SpawnObject<BravoViewport>();
}

BravoEngine::~BravoEngine()
{
}

void BravoEngine::StartGame()
{
	if ( Viewport )
		Viewport->GameLoop();
}

void BravoEngine::StopGame()
{
	if ( Viewport )
	{
		Viewport->ShutDown();
		delete Viewport;
		delete Input;
	}
}

void BravoEngine::Tick(float DeltaTime)
{
	for ( auto& it : TickableObjects )
		it.second->Update(DeltaTime);
}

void BravoEngine::DestroyObject(BravoObjectHandle InHandle)
{
	auto found = Objects.find(InHandle);
	if ( found == Objects.end() ) return;

	delete found->second;

	Objects.erase(InHandle);
	TickableObjects.erase(InHandle);
	Viewport->RemoveActor(InHandle);
}