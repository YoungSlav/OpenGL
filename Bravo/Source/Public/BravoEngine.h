#pragma once
#include "stdafx.h"
#include "SharedFromThis.h"

#include "BravoObject.h"
#include "BravoActor.h"

#include "BravoGameInstance.h"

#define TEST_MEMORY_LEAKS 0

// TODO:
/*
*	GRAPHICS:
* create some cool looking scene. kinda tired and bored looking at fucking cubes
* billboarding
* bloom
* proper anti aliasing
* landscape rendering (height maps)
* grass
* animations
* splines?
* frustum culling
* occlusion culling would be cool AF
* transparent objects (OIT)
* 
* 
*	ENGINE:
* Objects serialization deserialization   (cerial?)
* Physics engine integration (reactphysics3d)
* refactor class initialization through constructors
* 
*	EDITOR:
* Editor game instance 
* multiple projects in solution: Engine, Game
* editor UI
* */



class BravoInput;
class BravoEngine : public BravoObject
{
public:

	BravoEngine(const std::string& _Name) :
		BravoObject(0, _Name, nullptr, nullptr)
	{}

	template <typename Class>
	std::shared_ptr<Class> SpawnGameInstance(const std::string& _Name)
	{
		static_assert(std::is_base_of_v<BravoGameInstance, Class>);
		return NewObject<Class>(_Name);
	}

	void GameLoop();
	void StopGame();
		
	void RegisterObject(std::shared_ptr<BravoObject> Object);
	std::shared_ptr<BravoObject> FindObjectByHandle(const BravoHandle& Handle);
	BravoHandle GenerateNewHandle() { return ++LastUsedHandle; }
	void DestroyObject(std::shared_ptr<BravoObject> Object);

	void SetCamera(std::shared_ptr<class BravoCamera> _Camera);

	inline std::shared_ptr<class BravoViewport> GetViewport() const { return Viewport; }
	inline std::shared_ptr<class BravoInput> GetInput() const { return Input; }
	inline std::shared_ptr<class BravoLightManager> GetLightManager() const { return LightManager; }
	inline std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager; }
	inline std::shared_ptr<class BravoCamera> GetCamera() const { return Camera.expired() ? nullptr : Camera.lock(); }
	inline std::shared_ptr<class BravoSelectionManager> GetSelectionManager() const { return SelectionManager; }

	static std::shared_ptr<BravoEngine> GetEngine();

	inline bool IsShutingDown() const { return bRequestExit; }
	void ShutDown() { bRequestExit = true; }

protected:
	bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	void FireFreshBeginPlays();

	void Tick(float DeltaTime);
	
private:

	// freshly created objects awaiting BeginPlay call
	std::list< std::shared_ptr<BravoObject> > FreshObjects;

	// object managing
	std::list< std::shared_ptr<BravoObject> > Objects;
	std::list< std::shared_ptr<class BravoActor> > Actors;
	std::map<BravoHandle, std::shared_ptr<BravoObject>> HandleToObject;
	std::list< std::shared_ptr<IBravoTickable> > TickableObjects;

	std::shared_ptr<class BravoViewport> Viewport;
	std::shared_ptr<class BravoAssetManager> AssetManager = nullptr;
	std::shared_ptr<class BravoInput> Input;
	std::shared_ptr<class BravoLightManager> LightManager;
	std::shared_ptr<class BravoSelectionManager> SelectionManager;
	
	std::weak_ptr<class BravoCamera> Camera;

	BravoHandle LastUsedHandle = 0;

	float GameTime = 0.0f;
	bool bRequestExit = false;

	float FPSLimit = 60.0f;
};