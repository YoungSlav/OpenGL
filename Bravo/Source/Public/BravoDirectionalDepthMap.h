#pragma once


#include "stdafx.h"
#include "BravoObject.h"
#include "BravoShader.h"
#include "BravoDepthMap.h"


class BravoDirectionalDepthMap : public BravoDepthMapNew
{
public:
	void Setup(int32 LayersCount);
	void Render(int32 Layer, const struct BravoDirectionalLightShaderData& CasterData);
	
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

	int32 Layers = 0;
};