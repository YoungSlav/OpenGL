#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "BravoShader.h"
//#include "BravoDepthMap.h"


// delete me and move to BravoDepthMap.h
class BravoDepthMap2 : public BravoObject
{
public:


	virtual void Use(BravoShaderPtr OnShader) = 0;
	virtual void StopUsage() = 0;

protected:
	virtual void OnDestroy() override
	{
		DepthMapShader->ReleaseFromGPU();
		BravoObject::OnDestroy();
	}

	const uint32 Size = 2048;
	std::shared_ptr<class BravoShader> DepthMapShader;
};


class BravoSpotDepthMap : public BravoDepthMap2
{
public:
	void Setup(int32 CastersCount);
	void Render(int32 Layer, const struct BravoSpotLightShaderData& CasterData);

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