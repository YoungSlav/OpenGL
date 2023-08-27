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

	std::shared_ptr<class BravoDirectionalLightActor> GetDirectionalLight() const { return DirectionalLight; }

	int32 GetSpotDepthMapLayersCount() const { return (int32)SpotLights.size(); }

protected:
	virtual bool Initialize_Internal() override;

private:

private:

	std::vector< std::shared_ptr<class BravoSpotLightActor>> SpotLights;
	std::shared_ptr<class BravoSpotLightShaderDataCollection> SpotLightsDataCollection = nullptr;

	std::shared_ptr<class BravoDirectionalLightActor> DirectionalLight;
	std::vector< std::shared_ptr<class BravoPointLightActor>> PointLights;
};