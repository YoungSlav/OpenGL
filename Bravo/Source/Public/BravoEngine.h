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
* improve outline shader
* bloom
* proper anti aliasing
* landscape rendering (height maps)
* frustum culling
* occlusion culling would be cool AF
* transparent objects
* 
* 
* 
*	ENGINE:
* move rendering stuff into separate class
* refactor render target and create separate object for post process.
* Multiline text 
* Widget clicking 
* Edit text 
* Buttons 
* Json integration 
* Objects serialization deserialization 
* Outline shader?
* Gizmo for actor moving 
* Physics engine integration (reactphysics3d)
*
* 
*	EDITOR:
* Editor game instance 
* multiple projects in solution: Engine, Game
* world editor? (json?)
* serialization?
* splines?
* animations?
* */

typedef MulticastDelegate<const glm::ivec2&> OnResizeSignature;

class BravoInput;
class BravoEngine : public BravoObject
{
public:
	template <typename Class>
	std::shared_ptr<Class> SpawnGameInstance(const std::string& _Name)
	{
		static_assert(std::is_base_of_v<BravoGameInstance, Class>);
		return NewObject<Class>(_Name, Self<BravoEngine>());
	}

	void GameLoop();
	void StopGame();
	
	void RenderSelectionIDs() const;
	void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) const;
	
	void RegisterObject(std::shared_ptr<BravoObject> Object);
	std::shared_ptr<BravoObject> FindObjectByHandle(const BravoHandle& Handle);
	BravoHandle GenerateNewHandle() { return ++LastUsedHandle; }
	void DestroyObject(std::shared_ptr<BravoObject> Object);

	void SetCamera(std::shared_ptr<class BravoCamera> _Camera);

	inline std::shared_ptr<class BravoInput> GetInput() const { return Input; }
	inline std::shared_ptr<class BravoLightManager> GetLightManager() const { return LightManager; }
	inline std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager; }
	inline std::shared_ptr<class BravoHUD> GetHUD() const { return HUD; }
	inline std::shared_ptr<class BravoCamera> GetCamera() const { return Camera.expired() ? nullptr : Camera.lock(); }
	inline const glm::ivec2& GetViewportSize() const { return ViewportSize; }
	inline std::shared_ptr<class BravoSelectionManager> GetSelectionManager() const { return SelectionManager; }

	// TODO: get rid of this nasty hack. implement propper render buffer management
	void BindVieportRenderBuffer();


	static std::shared_ptr<BravoEngine> GetEngine();

	OnResizeSignature OnResizeDelegate;

protected:
	bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	void CreateOpenGLWindow();

	void Tick(float DeltaTime);
	void UpdateViewport();
	void Resize(const glm::ivec2& InViewportSize);

	// input
	static void Framebuffer_size_callback(struct GLFWwindow* window, int32 width, int32 height);

private:
	glm::ivec2 ViewportSize = glm::ivec2(1024, 768);

	struct GLFWwindow* Window = nullptr;

	// object managing
	std::vector< std::shared_ptr<BravoObject> > Objects;
	std::map<BravoHandle, std::shared_ptr<BravoObject>> HandleToObject;
	std::vector< std::shared_ptr<IBravoTickable> > TickableObjects;
	std::vector< std::shared_ptr<class IBravoRenderable> > RenderableObjects;
	std::vector< std::shared_ptr<class BravoActor> > Actors;

	std::shared_ptr<class BravoAssetManager> AssetManager = nullptr;
	std::shared_ptr<class BravoInput> Input;
	std::shared_ptr<class BravoLightManager> LightManager;
	std::shared_ptr<class BravoRenderTarget> ViewportRenderTarget;
	std::shared_ptr<class BravoHUD> HUD;
	std::shared_ptr<class BravoSelectionManager> SelectionManager;
	std::shared_ptr<class BravoOutlineManager> OutlineManager;


	std::weak_ptr<class BravoCamera> Camera;


	BravoHandle LastUsedHandle = 0;

	float GameTime = 0.0f;
	bool bRequestExit = false;

	float FPSLimit = 60.0f;
};