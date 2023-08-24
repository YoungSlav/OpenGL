#pragma once

#include "stdafx.h"

#include "BravoLightActor.h"
#include "BravoShadowMap.h"

struct BravoSpotLightConstants
{
	//										   0     1       2       3       4       5       6        7        8        9        10       11
	const std::vector<float> Distance =		{ 7.0f,	13.0f,	20.0f,	32.0f,	50.0f,	65.0f,	100.0f,	 160.0f,  200.0f,  325.0f,  600.0f,  3250.0f	};
	const std::vector<float> Constant =		{ 1.0f, 1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	 1.0f,	  1.0f,    1.0f,    1.0f,    1.0		};
	const std::vector<float> Linear =		{ 0.7f, 0.35f,	0.22f,	0.14f,	0.09f,	0.07f,	0.045f,	 0.027f,  0.022f,  0.014f,  0.007f,  0.0014f	};
	const std::vector<float> Quadratic =	{ 1.8f, 0.44f,	0.20f,	0.07f,	0.032f, 0.017f, 0.0075f, 0.0028f, 0.0019f, 0.0007f, 0.0002f, 0.000007f	};
};


struct BravoSpotLightSettings
{
	float CutOff = 30.0f;
	float OuterCutOff = 45.0f;

	uint32 Intencity = 0;
};

class BravoShadowMapSpot : public BravoShadowMap
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
