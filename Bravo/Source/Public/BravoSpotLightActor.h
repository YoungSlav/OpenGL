#pragma once

#include "stdafx.h"

#include "BravoLightActor.h"


struct BravoSpotLightSettings
{
	float CutOff = 15.0f;
	float OuterCutOff = 20.0f;

	float Intencity = 100.0f;
};

struct BravoSpotLightShaderData
{
	alignas(16) glm::vec3 Color;

	alignas(16) glm::vec3 Position;
	alignas(16) glm::vec3 Direction;

	float CutOff = 0.0f;
	float OuterCutOff = 0.0f;

	float FarPlane = 0.0f;

	alignas(16) glm::mat4 LightSpaceMatrix;
};


class BravoSpotLightActor : public BravoLightActor
{
public:
	template <typename... Args>
	BravoSpotLightActor(Args&&... args) :
		BravoLightActor(std::forward<Args>(args)...)
	{}


	void SetSettings(BravoSpotLightSettings _Settings);
	inline const BravoSpotLightSettings& GetSettings() const { return Settings; }

	

	void GetShaderData(BravoSpotLightShaderData& OutShaderData) const;

protected:
	virtual bool Initialize_Internal() override;

	void GetLightSpaceTransformationMatrix(glm::mat4& OutTransformationMatrix) const;

private:

	BravoSpotLightSettings Settings;
};
