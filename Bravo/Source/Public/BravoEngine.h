#pragma once
#include "stdafx.h"
#include "SharedFromThis.h"

#include "BravoObject.h"
#include "BravoActor.h"

#include "BravoGameInstance.h"

// TODO:
/*
* 
* 2d camera
* camera refactoring (engine should accept camera rather then spawning)
* Move geometries to separate component classes
*
* !!!!!!!!! Fix shadows !!!!!!!!!
* refactor lights (more parameters?)
* 
* Input refactoring (utilize delegates)
* Input text widget
* widgets mouse clicking
* console commands?
* 
*  
* reactphysics3d
* world editor? (json?)
* serialization?
* splines?
* particle systems?
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
		return NewObject<Class>(Self<BravoEngine>(), _Name);
	}

	void GameLoop();
	void StopGame();
	
	void RenderDepthMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& LightPosition) const;
	
	void RegisterObject(std::shared_ptr<BravoObject> Object);
	BravoHandle GenerateNewHandle() { return ++LastUsedHandle; }
	void DestroyObject(std::shared_ptr<BravoObject> Object);

	std::shared_ptr<class BravoInput> GetInput() const { return Input.expired() ? nullptr : Input.lock(); }
	std::shared_ptr<class BravoLightManager> GetLightManager() const { return LightManager.expired() ? nullptr : LightManager.lock(); }
	std::shared_ptr<class BravoCamera> GetCamera() const { return Camera.expired() ? nullptr : Camera.lock(); }
	std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager; }
	std::shared_ptr<class BravoHUD> GetHUD() const { return HUD.expired() ? nullptr : HUD.lock(); }
	const glm::ivec2& GetViewportSize() const { return ViewportSize; }

protected:
	bool Initialize_Internal() override;


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
	std::vector< std::shared_ptr<class BravoActor> > Actors;

	std::shared_ptr<class BravoAssetManager> AssetManager = nullptr;
	

	std::weak_ptr<class BravoLightManager> LightManager;
	std::weak_ptr<class BravoCamera> Camera;
	std::weak_ptr<class BravoRenderTarget> ViewportRenderTarget;
	std::shared_ptr<class BravoRenderTarget> GetViewportRenderTarget() const { return ViewportRenderTarget.lock(); }
	std::weak_ptr<class BravoHUD> HUD;

	std::weak_ptr<class BravoInput> Input;

	BravoHandle LastUsedHandle = 0;

	float GameTime = 0.0f;
	bool bRequestExit = false;
};