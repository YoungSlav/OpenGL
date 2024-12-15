#pragma once
#include "stdafx.h"
#include "BravoPostProcess.h"

class FluidPostProcess : public BravoPostProcess
{
public:
	template <typename... Args>
	FluidPostProcess(std::shared_ptr<class FluidSimulation3D> _Simulation, std::shared_ptr<class BravoBoundingBox> _Bounds, Args&&... args) :
		BravoPostProcess(std::forward<Args>(args)...),
		Simulation(_Simulation),
		BoundingBox(_Bounds)
	{}

	std::shared_ptr<class BravoShaderAsset> GetComputeShader() const { return RayMarchingCompute; };

protected:
	virtual bool Initialize_Internal() override;
	virtual bool EnsureReady() override;
	virtual void Render_Internal() override;

	void CreateDensitiesMap();
	void OnBoundingBoxTransofrmUpdated(const class IBravoTransformable*);


	void OnViewportResize(const glm::ivec2& NewSize);


	// TODO: clear on destroy

private:
	std::shared_ptr<class BravoRenderTarget> RenderTarget;
	std::shared_ptr<class BravoShaderAsset> RayMarchingCompute;
	std::shared_ptr<class BravoShaderAsset> DensitiesSampleCompute;

	std::shared_ptr<class FluidSimulation3D> Simulation;
	std::shared_ptr<class BravoBoundingBox> BoundingBox;

	GLuint DensitiesMapBuffer = 0;

	float DensitiesResolution = 0.05;
	glm::ivec3 DensitiesMapSize;
};
