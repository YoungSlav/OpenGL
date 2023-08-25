#pragma once

#include "stdafx.h"

#include "BravoLightActor.h"
#include "BravoDepthMap.h"


struct BravoPointLightSettings
{
	

	uint32 Intencity = 6;
};

class BravoDepthMapPoint : public BravoDepthMap
{
public:
	virtual void Setup(const uint32 Size) override;
	virtual void Apply(BravoShaderPtr OnShader, const std::string& Path) override;
	virtual void StopUsage() override;
	virtual void Render(std::shared_ptr<class BravoLightActor> Owner) override;

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:

	std::shared_ptr<class BravoPointLightActor> PointLightOwner;

	std::vector<glm::mat4> LightSpaceMatricies;
	
	int32 TextureUnit = -1;
	uint32 DepthMapFBO = 0;
	uint32 DepthCubemap = 0;
};

class BravoPointLightActor : public BravoLightActor
{
public:
	virtual void Apply(BravoShaderPtr OnShader) override;

	void SetSettings(BravoPointLightSettings _Settings);
	inline const BravoPointLightSettings& GetSettings() const { return Settings; }

protected:
	virtual bool Initialize_Internal() override;

private:
	BravoPointLightSettings Settings;
};
