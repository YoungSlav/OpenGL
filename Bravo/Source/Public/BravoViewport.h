#pragma once
#include "stdafx.h"
#include "BravoObject.h"


class BravoActor;
class BravoCamera;
class BravoEngine;
class BravoLightActor;
class BravoRenderTarget;
class BravoLightManager;

// TODO: merge with engine class

class BravoViewport : public BravoObject
{
	friend class BravoEngine;
public:
	void SetCamera(BravoCamera* InCamera);
	BravoCamera* GetCamera() const { return Camera; }

	const glm::ivec2& GetViewportSize() const { return ViewportSize; }
	void DrawDepthMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& Position) const;

	void BravoViewport::AddActor(BravoActor* InActor);
	void RemoveActor(BravoObjectHandle InHandle);

	void SetMouseEnabled(bool bNewMouseEnabled) const;

	class BravoLightManager* GetLightManager() const { return LightManager; }

	const float GetMinDrawingDistance() const { return MinDrawDistance; }
	const float GetMaxDrawingDistance() const { return MaxDrawDistance; }

protected:
	virtual void Init() override;
	virtual void OnDestroy() override;

	virtual void CreateOpenGLWindow();
	virtual void CreateDefaultCamera();

	virtual void UpdateViewport();
	
	void GameLoop();
	void ShutDown();

private:
	static void Framebuffer_size_callback(struct GLFWwindow* window, int width, int height);
	static void ProcessInput(GLFWwindow *window);
	static void Scroll_callback(struct GLFWwindow* window, double xoffset, double yoffset);
	static void Mouse_callback(struct GLFWwindow* window, double xpos, double ypos);

	void Resize(const glm::ivec2& InViewportSize);


protected:
	struct GLFWwindow* Window = nullptr;
	glm::ivec2 ViewportSize;

	std::vector< BravoActor* > Actors;

	BravoLightManager* LightManager;

	BravoCamera* Camera = nullptr;

	BravoRenderTarget* PostProcessRenderTarget;

	float MinDrawDistance = 0.1f;
	float MaxDrawDistance = 100.0f;

private:
	float OldGameTime = 0.0f;
	bool bShutDown = false;

};
