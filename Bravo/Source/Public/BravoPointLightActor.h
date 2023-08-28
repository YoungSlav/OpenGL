#pragma once

#include "stdafx.h"
#include "BravoLightActor.h"


struct BravoPointLightSettings
{
	uint32 Intencity = 6;
};

struct BravoPointLightShaderData
{
	alignas(16) glm::vec3 AmbientLight;
	alignas(16) glm::vec3 DiffuseLight;
	alignas(16) glm::vec3 SpecularLight;

	alignas(16) glm::vec3 Position;

	float Constant = 0.0f;
	float Linear = 0.0f;
	float Quadric = 0.0f;
	
	float FarPlane = 0.0f;
};

class BravoPointLightActor : public BravoLightActor
{
public:
	void SetSettings(BravoPointLightSettings _Settings);
	inline const BravoPointLightSettings& GetSettings() const { return Settings; }

	void GetShaderData(BravoPointLightShaderData& OutShaderData) const;
protected:
	virtual bool Initialize_Internal() override;

private:
	BravoPointLightSettings Settings;
};
