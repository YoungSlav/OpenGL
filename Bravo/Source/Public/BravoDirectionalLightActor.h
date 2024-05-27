#pragma once

#include "stdafx.h"

#include "BravoLightActor.h"

struct BravoDirectionalLightSettings
{
	std::vector<float> ShadowCascadeLevels { 0.02f, 0.05f, 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.5f, 0.75f, 0.8f, 1.0f };
	float FrustrumMultiplicator = 5.0f;
	float Intencity = 1.0f;
};

struct BravoDirectionalLightShaderData
{
	alignas(16) glm::vec3 Ambient;
	alignas(16) glm::vec3 Color;
	
	alignas(16) glm::vec3 Direction;

	int32 LayerOffset;
	int32 CascadeCount;
	float cascadePlaneDistance;

	alignas(16) glm::mat4 LightSpaceMatrix;
};

class BravoDirectionalLightActor : public BravoLightActor
{
public:
	template <typename... Args>
	BravoDirectionalLightActor(const BravoDirectionalLightSettings& _Settings, Args&&... args) :
		BravoLightActor(std::forward<Args>(args)...),
		Settings(_Settings)
	{}


	void GetShaderData(std::vector<BravoDirectionalLightShaderData>& OutShaderData) const;

	void SetSettings(const BravoDirectionalLightSettings& _Settings) { Settings = _Settings; }
	const BravoDirectionalLightSettings& GetSettings() const { return Settings; }

	void SetAmbientLightColor(const glm::vec3& _Color) { AmbientColor = _Color; }
	const glm::vec3& GetAmbientLightColor() const { return AmbientColor; }

protected:
	virtual bool Initialize_Internal() override;

private:
	glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane) const;

private:

	glm::vec3 AmbientColor;

	BravoDirectionalLightSettings Settings;
};