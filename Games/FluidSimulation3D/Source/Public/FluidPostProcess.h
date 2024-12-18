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

public:
	float DensityOffset = 30.0f;
	float DensityMultiplier = 0.001f;
	float MarchingRayStep = 0.05f;
	float LightMarchingRayStep = 0.5f;
	float DensitiesResolution = 0.05;

	float SurfaceStepsTreashold = 100.0f;

	glm::vec3 DirToLight = glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f));

	glm::vec3 ScatteringCoefficients = glm::vec3(32.0f, 16.0f, 2.0f)*0.25f;

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

	
	glm::ivec3 DensitiesMapSize;
};
