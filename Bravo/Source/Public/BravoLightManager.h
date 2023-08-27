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

	void UpdateLightsShaderData();

	void ApplyLights(std::shared_ptr<class BravoShader> Shader);
	void ResetLightsUsage();

	int32 GetSpotDepthMapLayersCount() const { return (int32)SpotLights.size(); }

protected:
	virtual bool Initialize_Internal() override;

private:

private:
	std::vector<std::shared_ptr<class BravoDirectionalLightActor>> DirectionalLights;
	std::shared_ptr<class BravoDirectionalLightShaderDataCollection> DirectionalLightsDataCollection = nullptr;

	std::vector< std::shared_ptr<class BravoSpotLightActor>> SpotLights;
	std::shared_ptr<class BravoSpotLightShaderDataCollection> SpotLightsDataCollection = nullptr;

	std::vector< std::shared_ptr<class BravoPointLightActor>> PointLights;
};