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

	std::shared_ptr<class BravoDirectionalLightActor> GetDirectionalLight() const { return DirectionalLight; }

	int32 GetSpotDepthMapLayersCount() const { return (int32)SpotLights.size(); }

private:

	std::shared_ptr<class BravoSpotDepthMap> SpotDepthMap;	// TODO: initialize and re-init on new spot light

	void UpdateShaderPaths();

	std::shared_ptr<class BravoDirectionalLightActor> DirectionalLight;
	std::vector< std::shared_ptr<class BravoSpotLightActor>> SpotLights;
	std::vector< std::shared_ptr<class BravoPointLightActor>> PointLights;
};