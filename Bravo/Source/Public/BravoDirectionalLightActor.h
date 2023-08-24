#pragma once

#include "stdafx.h"

#include "BravoLightActor.h"
#include "BravoShadowMap.h"

struct BravoDirectionalLightSettings
{
	std::vector<float> ShadowCascadeLevels { 0.02f, 0.05f, 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.5f, 0.75f, 0.8f, 1.0f };
	float FrustrumMultiplicator = 5.0f;
};

class BravoShadowMapDirectional : public BravoShadowMap
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
	std::vector<glm::mat4> GetLightSpaceMatrices(const glm::vec3& LightDirection) const;
	glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane, const glm::vec3& LightDirection) const;

private:
	uint32 DepthMapFBO = 0;
	uint32 DepthMapsTextures = 0;
	int32 TextureUnit = -1;
	uint32 MatricesUBO = 0;

	std::vector<glm::mat4> CacheLightMatrices;

	std::shared_ptr<class BravoDirectionalLightActor> DirLightOwner;
};

class BravoDirectionalLightActor : public BravoLightActor
{
public:

	virtual void Use(BravoShaderPtr OnShader) override;

	void SetSettings(BravoDirectionalLightSettings _Settings);
	inline const BravoDirectionalLightSettings& GetSettings() const { return Settings; }

protected:
	virtual bool Initialize_Internal() override;

private:
	BravoDirectionalLightSettings Settings;
};