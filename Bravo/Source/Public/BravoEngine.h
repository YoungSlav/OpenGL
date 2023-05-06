#pragma once
#include "stdafx.h"
#include "SharedFromThis.h"

#include "BravoObject.h"
#include "BravoActor.h"


class BravoInput;

class BravoEngine : public SharedFromThis
{
public:
	BravoEngine();
	void Init();

	void GameLoop();
	void Tick(float DeltaTime);
	void StopGame();
	
	void DrawShadowMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& LightPosition) const;

	
	template <typename Class>
	std::shared_ptr<Class> SpawnObject()
	{
		static_assert(std::is_base_of_v<BravoObject, Class>);
		
		std::shared_ptr<BravoObject> newObject = std::shared_ptr<BravoObject>(new Class(Self<BravoEngine>(), ++LastUsedHandle));
		RegisterObject(newObject);
		newObject->Init();
		return std::dynamic_pointer_cast<Class>(newObject);
	}

	
	void DestroyObject(std::shared_ptr<BravoObject> Object);

	std::shared_ptr<class BravoInput> GetInput() const;
	std::shared_ptr<class BravoLightManager> GetLightManager() const;
	std::shared_ptr<class BravoCamera> GetCamera() const;
	std::shared_ptr<class BravoAssetManager> GetAssetManager() const { return AssetManager; }

	const glm::ivec2& GetViewportSize() const { return ViewportSize; }

	void SetMouseEnabled(bool bNewMouseEnabled) const;

private:
	void CreateOpenGLWindow();
	void UpdateViewport();

	void Resize(const glm::ivec2& InViewportSize);

	void RegisterObject(std::shared_ptr<BravoObject> Object);

	// input
	static void Framebuffer_size_callback(struct GLFWwindow* window, int width, int height);
	static void ProcessInput(GLFWwindow *window);
	static void Scroll_callback(struct GLFWwindow* window, double xoffset, double yoffset);
	static void Mouse_callback(struct GLFWwindow* window, double xpos, double ypos);

private:
	glm::ivec2 ViewportSize;

	struct GLFWwindow* Window = nullptr;

	// object managing
	std::map< BravoHandle, std::shared_ptr<BravoObject> > Objects;
	std::map< BravoHandle, std::weak_ptr<BravoTickable> > TickableObjects;
	std::vector< std::weak_ptr<class BravoActor> > Actors;

	std::shared_ptr<class BravoAssetManager> AssetManager = nullptr;
	

	std::weak_ptr<class BravoLightManager> LightManager;
	std::weak_ptr<class BravoCamera> Camera;
	std::weak_ptr<class BravoRenderTarget> ViewportRenderTarget;
	std::shared_ptr<class BravoRenderTarget> GetViewportRenderTarget() const;

	std::weak_ptr<class BravoInput> Input;

	BravoHandle LastUsedHandle = 0;

	float GameTime = 0.0f;
	bool bRequestExit = false;
};
