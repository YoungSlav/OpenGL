#pragma once
#include "BravoActor.h"
#include "BravoStaticMeshAsset.h"
#include "BravoShaderAsset.h"
#include "BravoDepthMap.h"
#include "IBravoRenderable.h"

struct BravoLightAttenuationConstants
{
	//	Intensity:							   0     1       2       3       4       5       6        7        8        9        10       11
	const std::vector<float> Distance =		{ 7.0f,	13.0f,	20.0f,	32.0f,	50.0f,	65.0f,	100.0f,	 160.0f,  200.0f,  325.0f,  600.0f,  3250.0f	};
	const std::vector<float> Constant =		{ 1.0f, 1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	 1.0f,	  1.0f,    1.0f,    1.0f,    1.0		};
	const std::vector<float> Linear =		{ 0.7f, 0.35f,	0.22f,	0.14f,	0.09f,	0.07f,	0.045f,	 0.027f,  0.022f,  0.014f,  0.007f,  0.0014f	};
	const std::vector<float> Quadratic =	{ 1.8f, 0.44f,	0.20f,	0.07f,	0.032f, 0.017f, 0.0075f, 0.0028f, 0.0019f, 0.0007f, 0.0002f, 0.000007f	};
};


class BravoLightActor : public BravoActor
{
public:
	BravoLightActor() = default;

	virtual void Apply(std::shared_ptr<BravoShaderAsset> OnShader);
	virtual void StopUsage();

	void SetLightColor(const glm::vec3& InLightColor) { LightColor = InLightColor; }
	const glm::vec3& GetLightColor() const { return LightColor; }
	

	void SetShaderPath(const std::string& InPath) { Path = InPath; }
	virtual void UpdateDepthMap();


protected:
	virtual void OnDestroy() override;


public:
	static BravoLightAttenuationConstants LightAttenuationConstants;

protected:
	
	std::string Path = "";
	glm::vec3 LightColor;

	std::shared_ptr<BravoDepthMap> DepthMap;

};

