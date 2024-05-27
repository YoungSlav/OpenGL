#pragma once
#include "BravoPlayer.h"
#include "BravoShaderAsset.h"

class BravoDepthMap : public BravoObject
{
public:
	template <typename... Args>
	BravoDepthMap(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}

	virtual void Setup(const int32 LayersCount, const uint32 TextureSize) = 0;
	virtual void Use(std::shared_ptr<BravoShaderAsset> OnShader) = 0;
	virtual void StopUsage() = 0;

protected:

	int32 Layers = 0;
	uint32 Size = 2048;
	std::shared_ptr<class BravoShaderAsset> DepthMapShader;
};