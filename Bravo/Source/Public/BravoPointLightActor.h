#pragma once

#include "stdafx.h"
#include "BravoLightActor.h"


struct BravoPointLightSettings
{
	float Intencity = 100.0f;
};

struct BravoPointLightShaderData
{
	alignas(16) glm::vec3 Color;

	alignas(16) glm::vec3 Position;

	float FarPlane = 100.0f;
};

class BravoPointLightActor : public BravoLightActor
{
public:
	template <typename... Args>
	BravoPointLightActor(Args&&... args) :
		BravoLightActor(std::forward<Args>(args)...)
	{}


	void SetSettings(BravoPointLightSettings _Settings);
	inline const BravoPointLightSettings& GetSettings() const { return Settings; }

	void GetShaderData(BravoPointLightShaderData& OutShaderData) const;
protected:
	virtual bool Initialize_Internal() override;

private:
	BravoPointLightSettings Settings;
};
