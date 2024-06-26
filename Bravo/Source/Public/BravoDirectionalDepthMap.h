#pragma once


#include "stdafx.h"
#include "BravoObject.h"
#include "BravoShaderAsset.h"
#include "BravoDepthMap.h"
#include "BravoDirectionalLightActor.h"

class BravoDirectionalDepthMap : public BravoDepthMap
{
public:
	template <typename... Args>
	BravoDirectionalDepthMap(Args&&... args) :
		BravoDepthMap(std::forward<Args>(args)...)
	{}


	void Setup(const int32 LayersCount, const uint32 TextureSize);
	void Render(const std::vector<BravoDirectionalLightShaderData>& CastersData);
	
	virtual void Use(std::shared_ptr<BravoShaderAsset> OnShader);
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