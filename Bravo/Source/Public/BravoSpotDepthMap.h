#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "BravoShader.h"
#include "BravoDepthMap.h"
#include "BravoSpotLightActor.h"

class BravoSpotDepthMap : public BravoDepthMapNew
{
public:
	virtual void Setup(const int32 LayersCount, const uint32 TextureSize) override;
	void Render(const std::vector<BravoSpotLightShaderData>& CastersData);

	virtual void Use(BravoShaderPtr OnShader);
	virtual void StopUsage();

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	void ClearGPUData();

private:

	int32 TextureUnit = -1;
	uint32 DepthMapFBO = 0;
	uint32 DepthMapsTextures = 0;
};