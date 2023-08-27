#pragma once

#include "stdafx.h"

#include "BravoLightActor.h"
#include "BravoDepthMap.h"

struct BravoDirectionalLightSettings
{
	std::vector<float> ShadowCascadeLevels { 0.02f, 0.05f, 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.5f, 0.75f, 0.8f, 1.0f };
	float FrustrumMultiplicator = 5.0f;
};

struct BravoDirectionalLightShaderData
{

	alignas(16) glm::vec3 AmbientLight;
	alignas(16) glm::vec3 DiffuseLight;
	alignas(16) glm::vec3 SpecularLight;

	alignas(16) glm::vec3 Direction;

	int32 LayerOffset;
	int32 CascadeCount;
	float cascadePlaneDistance;

	alignas(16) glm::mat4 LightSpaceMatrix;
};

class BravoDirectionalLightActor : public BravoLightActor
{
public:

	void GetShaderData(std::vector<BravoDirectionalLightShaderData>& OutShaderData) const;

	void SetSettings(BravoDirectionalLightSettings _Settings) { Settings = _Settings; }
	const BravoDirectionalLightSettings& GetSettings() const { return Settings; }

protected:
	virtual bool Initialize_Internal() override;

private:
	glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane) const;

private:
	BravoDirectionalLightSettings Settings;
};