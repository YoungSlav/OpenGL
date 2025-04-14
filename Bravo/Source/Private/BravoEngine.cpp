#include "BravoEngine.h"
#include "BravoInput.h"
#include "BravoCamera.h"
#include "BravoLightManager.h"
#include "BravoRenderTarget.h"
#include "BravoAssetManager.h"
#include "openGL.h"
#include "BravoHUD.h"
#include "IBravoRenderable.h"
#include "BravoSelectionManager.h"
#include "BravoViewport.h"
#include "BravoTimeManager.h"

namespace GlobalEngine
{
	std::weak_ptr<BravoEngine> _Engine;
	std::shared_ptr<BravoEngine> Engine()
	{
		if( _Engine.expired() )
			return nullptr;
		return _Engine.lock();
	}
};


std::shared_ptr<BravoEngine> BravoEngine::GetEngine()
{
	return GlobalEngine::Engine();
}

bool BravoEngine::Initialize_Internal()
{
	GlobalEngine::_Engine = Self<BravoEngine>();
	
	Engine = Self<BravoEngine>();

	LastUsedHandle = GetHandle();

	TimeManager = NewObject<BravoTimeManager>("TimeManager");

	AssetManager = NewObject<BravoAssetManager>("AssetManager");
	
	Viewport = NewObject<BravoViewport>("Viewport");
	Viewport->Setup();

	SelectionManager = NewObject<BravoSelectionManager>("SelectionManager");

	Input = NewObject<BravoInput>("Input", Viewport->Window);



	LightManager = NewObject<BravoLightManager>("LightManager");


	

	return true;
}

void BravoEngine::SetCamera(std::shared_ptr<class BravoCamera> _Camera)
{
	Camera = _Camera;
}


void BravoEngine::GameLoop()
{
	while( !bRequestExit && !glfwWindowShouldClose(Viewport->Window) )
	{
		float newTime = (float)glfwGetTime();
		float DeltaTime = newTime - GameTime;
		GameTime = newTime;
		
		AssetManager->CheckPendingAssets();

		FireFreshBeginPlays();

		Tick(DeltaTime);
		
		if ( GetCamera() )
			GetCamera()->UpdateCamera();

		Viewport->UpdateViewport(DeltaTime);
		

		if ( Input )
			Input->ProcessInput(DeltaTime);

		while ((glfwGetTime()  - newTime) < (1.0 / FPSLimit)) { }
	}

	glfwTerminate();
}

void BravoEngine::FireFreshBeginPlays()
{
	for ( auto it : FreshObjects )
	{
		it->OnBeginPlay();
	}
	FreshObjects.clear();
}

void BravoEngine::StopGame()
{
	while( Objects.size() > 0 )
	{
		DestroyObject(*Objects.cbegin());
	}

	glfwSetWindowShouldClose(Viewport->Window, true);
}

void BravoEngine::Tick(float DeltaTime)
{
	for ( auto it : TickableObjects )
		it->Update(DeltaTime);
}


void BravoEngine::RegisterObject(std::shared_ptr<BravoObject> newObject)
{
	Objects.push_back(newObject);
	
	FreshObjects.push_back(newObject);

	HandleToObject.insert({newObject->GetHandle(), newObject});
	
	if ( std::shared_ptr<IBravoTickable> asTickable = std::dynamic_pointer_cast<IBravoTickable>(newObject) )
	{
		TickableObjects.push_back(asTickable);
	}
	if ( std::shared_ptr<BravoActor> asActor = std::dynamic_pointer_cast<BravoActor>(newObject) )
	{
		Actors.push_back(asActor);

		if ( std::shared_ptr<BravoLightManager> lightManager = GetLightManager() )
		{
			if ( std::shared_ptr<BravoLightActor> asLight = std::dynamic_pointer_cast<BravoLightActor>(asActor) )
			{
				lightManager->RegisterLightActor(asLight);
			}
		}
	}
	if ( std::shared_ptr<IBravoRenderable> asRenderable = std::dynamic_pointer_cast<IBravoRenderable>(newObject) )
	{
		Viewport->RegisterRenderable(asRenderable);
	}
}

std::shared_ptr<BravoObject> BravoEngine::FindObjectByHandle(const BravoHandle& Handle)
{
	auto it = HandleToObject.find(Handle);
	if ( it != HandleToObject.end() )
		return it->second;
	return nullptr;
}

void BravoEngine::DestroyObject(std::shared_ptr<BravoObject> Object)
{
	if ( Input )
		Input->UnSubscribeAll(Object);

	HandleToObject.erase(Object->GetHandle());

	if ( std::shared_ptr<BravoActor> asActor = std::dynamic_pointer_cast<BravoActor>(Object) )
	{
		if ( std::shared_ptr<BravoLightManager> lightManager = GetLightManager() )
		{
			if ( std::shared_ptr<BravoLightActor> asLight = std::dynamic_pointer_cast<BravoLightActor>(asActor) )
				lightManager->RemoveLightActor(asLight);
		}
		Actors.erase(std::remove(Actors.begin(), Actors.end(), asActor), Actors.end());
	}

	if ( std::shared_ptr<IBravoTickable> asTiackble = std::dynamic_pointer_cast<IBravoTickable>(Object) )
		TickableObjects.erase(std::remove(TickableObjects.begin(), TickableObjects.end(), asTiackble), TickableObjects.end());


	if ( std::shared_ptr<IBravoRenderable> asRenderable = std::dynamic_pointer_cast<IBravoRenderable>(Object) )
		Viewport->RemoveRenderable(asRenderable);

	Objects.erase(std::remove(Objects.begin(), Objects.end(), Object), Objects.end());
}

void BravoEngine::OnDestroy()
{
	BravoObject::OnDestroy();

	Viewport.reset();
	Input.reset();
	LightManager.reset();
	
	SelectionManager.reset();
	
}