#pragma once

#include "stdafx.h"

#include "BravoLightActor.h"
#include "BravoDepthMap.h"


struct BravoSpotLightSettings
{
	float CutOff = 30.0f;
	float OuterCutOff = 45.0f;

	uint32 Intencity = 0;
};

class BravoDepthMapSpot : public BravoDepthMap
{
public:
	virtual void Setup(const uint32 Size) override;
	virtual void Use(BravoShaderPtr OnShader, const std::string& Path) override;
	virtual void StopUsage() override;
	virtual void Render(std::shared_ptr<class BravoLightActor> Owner) override;

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:

	std::shared_ptr<class BravoSpotLightActor> SpotLightOwner;

	glm::mat4 LightSpaceMatrix;
	
	int32 TextureUnit = -1;
	uint32 DepthMapFBO = 0;
	uint32 DepthMap = 0;
};

class BravoSpotLightActor : public BravoLightActor
{
public:
	virtual void Use(BravoShaderPtr OnShader) override;

	void SetSettings(BravoSpotLightSettings _Settings);
	inline const BravoSpotLightSettings& GetSettings() const { return Settings; }

protected:
	virtual bool Initialize_Internal() override;

private:
	BravoSpotLightSettings Settings;
};
