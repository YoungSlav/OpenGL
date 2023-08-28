#pragma once
#include "BravoPlayer.h"
#include "BravoShaderAsset.h"

class BravoDepthMap : public BravoObject
{
public:

	virtual void Setup(const uint32 Size) = 0;
	virtual void Render(std::shared_ptr<class BravoLightActor> Owner) = 0;

	virtual void Apply(std::shared_ptr<BravoShaderAsset> OnShader, const std::string& Path) = 0;
	virtual void StopUsage() = 0;

protected:
	virtual void OnDestroy() override;

	uint32 Size;
	std::shared_ptr<class BravoShaderAsset> DepthMapShader;
};

class BravoDepthMapNew : public BravoObject
{
public:
	virtual void Setup(const int32 LayersCount, const uint32 TextureSize) = 0;
	virtual void Use(std::shared_ptr<BravoShaderAsset> OnShader) = 0;
	virtual void StopUsage() = 0;

protected:

	int32 Layers = 0;
	uint32 Size = 2048;
	std::shared_ptr<class BravoShaderAsset> DepthMapShader;
};