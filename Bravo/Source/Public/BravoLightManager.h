#pragma once
#include "stdafx.h"

#include "BravoEngine.h"
#include "BravoObject.h"

#include "BravoLightActor.h"

class BravoLightManager : public BravoObject
{
public:
	void RegisterLightActor(BravoLightActor* LightActor);
	void RemoveLightActor(BravoObjectHandle InHandle);

	void UpdateLightsDepthMaps();

	void ApplyLights(std::shared_ptr<class BravoShader> Shader);
	void ResetLightsUsage();

private:

	void UpdateShaderPaths();

	BravoDirLightActor* DirectionalLight = nullptr;
	std::vector<BravoSpotLightActor*> SpotLights;
	std::vector<BravoPointLightActor*> PointLights;
};