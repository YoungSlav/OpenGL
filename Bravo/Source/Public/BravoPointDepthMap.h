#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "BravoShaderAsset.h"
#include "BravoDepthMap.h"
#include "BravoPointLightActor.h"

class BravoPointDepthMap : public BravoDepthMap
{
public:
	template <typename... Args>
	BravoPointDepthMap(Args&&... args) :
		BravoDepthMap(std::forward<Args>(args)...)
	{}


	virtual void Setup(const int32 LayersCount, const uint32 TextureSize) override;
	void Render(const std::vector<BravoPointLightShaderData>& CastersData);

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
