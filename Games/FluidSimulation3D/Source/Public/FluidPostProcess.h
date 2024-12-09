#pragma once
#include "stdafx.h"
#include "BravoPostProcess.h"

class FluidPostProcess : public BravoPostProcess
{
public:
	template <typename... Args>
	FluidPostProcess(Args&&... args) :
		BravoPostProcess(std::forward<Args>(args)...)
	{}

	std::shared_ptr<class BravoShaderAsset> GetComputeShader() const { return RayMarchingCompute; };

protected:
	virtual bool Initialize_Internal() override;
	virtual bool EnsureReady() override;
	virtual void Render_Internal() override;

	void OnViewportResize(const glm::ivec2& NewSize);


private:
	std::shared_ptr<class BravoRenderTarget> RenderTarget;
	std::shared_ptr<class BravoShaderAsset> RayMarchingCompute;
};
