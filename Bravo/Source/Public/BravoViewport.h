#pragma once
#include "stdafx.h"
#include "IBravoRenderable.h"
#include "BravoObject.h"

typedef MulticastDelegate<const glm::ivec2&> OnResizeSignature;

class BravoViewport : public BravoObject
{
	friend class BravoEngine;

public:
	template <typename... Args>
	BravoViewport(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	void UpdateViewport(float DeltaTime);
	void RenderSelectionIDs() const;
	void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) const;

	inline std::shared_ptr<class BravoHUD> GetHUD() const { return HUD; }
	inline const glm::ivec2& GetViewportSize() const { return ViewportSize; }

	void PushFramebuffer(uint32 Framebuffer, const glm::ivec2& Size);
	void PopFramebuffer();

	void AddPostProcess(std::shared_ptr<class BravoPostProcess> PP);

	void ToggleWireframe() { bWireframe = !bWireframe; }


	OnResizeSignature OnResizeDelegate;

	void DeProject(const glm::vec2& ScreenPos, glm::vec3& OutOrigin, glm::vec3& OutDirection);
	
protected:
	bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	void Setup();
	void CreateOpenGLWindow();

	void RegisterRenderable(std::shared_ptr<IBravoRenderable> Renderable);
	void RemoveRenderable(std::shared_ptr<IBravoRenderable> Renderable);

	struct RenderGroup
	{
		std::list<std::shared_ptr<class IBravoRenderable>> Renderables;
	};

	inline const std::shared_ptr<RenderGroup> GetRenderGroup(ERenderGroup GroupId) const
	{
		auto it = RenderGroups.find(GroupId);
		if ( it != RenderGroups.end() )
			return it->second;
		return nullptr;
	}

	inline std::shared_ptr<RenderGroup> AddRenderGroup(ERenderGroup GroupId)
	{
		if ( std::shared_ptr<RenderGroup> gr = GetRenderGroup(GroupId) )
			return gr;

		std::shared_ptr<RenderGroup> newGroup(new RenderGroup());
		RenderGroups.insert({GroupId, newGroup});
		return newGroup;
	}

	void Resize(const glm::ivec2& InViewportSize);

	static void Framebuffer_size_callback(struct GLFWwindow* window, int32 width, int32 height);

private:

	bool bWireframe = false;

	glm::ivec2 ViewportSize = glm::vec2(1440.0f, 900.0f);

	struct GLFWwindow* Window = nullptr;
	std::shared_ptr<class BravoRenderTarget> ViewportRenderTarget;
	std::stack<std::tuple<uint32, glm::ivec2>> FramebufferStack;

	std::map<ERenderGroup, std::shared_ptr<RenderGroup>> RenderGroups;

	std::shared_ptr<class BravoHUD> HUD;


	std::list<std::shared_ptr<class BravoPostProcess>> PostProcesses;
	std::shared_ptr<class BravoPostProcess_Outline> OutlinePP;
	std::shared_ptr<class BravoPostProcess_AntiAliasing> AntiAliasingPP;
};
