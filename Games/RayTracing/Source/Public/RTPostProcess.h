#pragma once
#include "stdafx.h"
#include "BravoPostProcess.h"

class RTPostProcess : public BravoPostProcess
{
public:
	template <typename... Args>
	RTPostProcess(Args&&... args) :
		BravoPostProcess(std::forward<Args>(args)...)
	{}

	void RegisterSkybox(std::shared_ptr<class BravoCubemapAsset> _CubeMap);
	void RegisterSceneComponent(std::shared_ptr<class RTSceneComponent> _NewComponent);

protected:
	virtual bool Initialize_Internal() override;
	virtual bool EnsureReady() override;
	virtual void Render_Internal() override;

	void OnViewportResize(const glm::ivec2& NewSize);
	void OnComponentDestroy(std::shared_ptr<BravoObject> _Object);
	void OnComponentUpdate(const RTSceneComponent* _Component);

	void UpdateSceneData();

private:
	std::shared_ptr<class BravoRenderTarget> RenderTarget;
	std::shared_ptr<class BravoShaderAsset> RayTracingCompute;

	std::map<BravoHandle, std::shared_ptr<class RTSceneComponent>> RegisteredComponents;

	std::shared_ptr<class BravoCubemapAsset> Skybox;
	bool bDirtyData = true;

	GLuint SceneComponentsSSBO = 0;
};
