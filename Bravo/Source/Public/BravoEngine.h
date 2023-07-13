#pragma once
#include "stdafx.h"
#include "SharedFromThis.h"

#include "BravoObject.h"
#include "BravoActor.h"

#include "BravoGameInstance.h"

#define TEST_MEMORY_LEAKS 0

// TODO:
/*
*
* Material refactoring. (to have colors OR textures)
* REFACTOR LIGHTS AND SHADOWS
* frustrum culling
* occlusion culling
* 
* Multiline text 3
* Widget clicking 3
* Edit text 8
* Buttons 1
* Json integration 1
* Objects serialization deserialization 8
* Editor game instance 2
* Input refactoring 4
* Pixel clicking actor selection 8
* Outline shader?
* Gizmo for actor moving 4
* Physics engine integration 20
* reactphysics3d
* 
*  
* world editor? (json?)
* serialization?
* splines?
* animations?
* */

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
	
	void RenderDepthMap(std::shared_ptr<class BravoShader> Shader) const;
	
	void RegisterObject(std::shared_ptr<BravoObject> Object);
	BravoHandle GenerateNewHandle() { return ++LastUsedHandle; }
	void DestroyObject(std::shared_ptr<BravoObject> Object);

	void SetCamera(std::shared_ptr<class BravoCamera> _Camera);

	inline std::shared_ptr<class BravoInput> GetInput() const { return Input; }
	inline std::shared_ptr<class BravoLightManager> GetLightManager() const { return LightManager; }
	inline std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager; }
	inline std::shared_ptr<class BravoHUD> GetHUD() const { return HUD; }
	inline std::shared_ptr<class BravoCamera> GetCamera() const { return Camera.expired() ? nullptr : Camera.lock(); }
	inline const glm::ivec2& GetViewportSize() const { return ViewportSize; }

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
	static void ProcessInput(GLFWwindow *window);
	static void Scroll_callback(struct GLFWwindow* window, double xoffset, double yoffset);
	static void Mouse_callback(struct GLFWwindow* window, double xpos, double ypos);
private:
	glm::ivec2 ViewportSize = glm::vec2(1024.0f, 768.0f);

	struct GLFWwindow* Window = nullptr;

	// object managing
	std::vector< std::shared_ptr<BravoObject> > Objects;
	std::vector< std::shared_ptr<BravoTickable> > TickableObjects;
	std::vector< std::shared_ptr<class BravoRenderable> > RenderableObjects;
	std::vector< std::shared_ptr<class BravoActor> > Actors;

	std::shared_ptr<class BravoAssetManager> AssetManager = nullptr;
	std::shared_ptr<class BravoInput> Input;
	std::shared_ptr<class BravoLightManager> LightManager;
	std::shared_ptr<class BravoRenderTarget> ViewportRenderTarget;
	std::shared_ptr<class BravoHUD> HUD;


	std::weak_ptr<class BravoCamera> Camera;


	BravoHandle LastUsedHandle = 0;

	float GameTime = 0.0f;
	bool bRequestExit = false;

	float FPSLimit = 100.0f;
};