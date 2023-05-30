#pragma once
#include "stdafx.h"

#include "BravoEngine.h"
#include "BravoObject.h"

#include "BravoLightActor.h"

class BravoLightManager : public BravoObject
{
public:

	void RegisterLightActor(std::shared_ptr<BravoLightActor> LightActor);
	void RemoveLightActor(std::shared_ptr<BravoLightActor> Actor);

	void UpdateLightsDepthMaps();

	void ApplyLights(std::shared_ptr<class BravoShader> Shader);
	void ResetLightsUsage();

private:



	void UpdateShaderPaths();

	std::shared_ptr<BravoDirLightActor> DirectionalLight;
	std::vector< std::shared_ptr<BravoSpotLightActor>> SpotLights;
	std::vector< std::shared_ptr<BravoPointLightActor>> PointLights;
};