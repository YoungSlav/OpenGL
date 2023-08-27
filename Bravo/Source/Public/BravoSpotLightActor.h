#pragma once

#include "stdafx.h"

#include "BravoLightActor.h"


struct BravoSpotLightSettings
{
	float CutOff = 15.0f;
	float OuterCutOff = 20.0f;

	uint32 Intencity = 6;
};

struct BravoSpotLightShaderData
{
	alignas(16) glm::mat4 LightSpaceMatrix;
	alignas(16) glm::vec3 AmbientLight;
	alignas(16) glm::vec3 DiffuseLight;
	alignas(16) glm::vec3 SpecularLight;

	alignas(16) glm::vec3 Position;
	alignas(16) glm::vec3 Direction;

	float CutOff = 0.0f;
	float OuterCutOff = 0.0f;

	// TODO: replace with IntencityIndex
	float Constant = 0.0f;
	float Linear = 0.0f;
	float Quadric = 0.0f;
	float FarPlane = 0.0f;

};


class BravoSpotLightActor : public BravoLightActor
{
public:
	void SetSettings(BravoSpotLightSettings _Settings);
	inline const BravoSpotLightSettings& GetSettings() const { return Settings; }

	

	void GetShaderData(BravoSpotLightShaderData& OutShaderData) const;

protected:
	virtual bool Initialize_Internal() override;

	void GetLightSpaceTransformationMatrix(glm::mat4& OutTransformationMatrix) const;

private:

	BravoSpotLightSettings Settings;
};
