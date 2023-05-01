#pragma once
#include "stdafx.h"

#include "BravoEngine.h"
#include "BravoObject.h"

#include "BravoLightActor.h"

class BravoLightManager : public BravoObject
{
public:
	BravoLightManager(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoObject(_Engine, _Handle)
	{}

	void RegisterLightActor(std::shared_ptr<BravoLightActor> LightActor);
	void RemoveLightActor(std::shared_ptr<BravoActor> Actor);

	void UpdateLightsDepthMaps();

	void ApplyLights(std::shared_ptr<class BravoShader> Shader);
	void ResetLightsUsage();

private:



	void UpdateShaderPaths();

	std::weak_ptr<BravoDirLightActor> DirectionalLight;
	std::vector< std::weak_ptr<BravoSpotLightActor>> SpotLights;
	std::vector< std::weak_ptr<BravoPointLightActor>> PointLights;
};