#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "BravoShader.h"
#include "BravoDepthMap.h"


// delete me and move to BravoDepthMap.h



class BravoSpotDepthMap : public BravoDepthMapNew
{
public:
	void Setup(int32 CastersCount);
	void Render(uint32 ShaderDataSSBO, int32 Count);

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