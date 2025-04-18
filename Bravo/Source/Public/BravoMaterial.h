#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoShaderAsset.h"
#include "BravoTextureAsset.h"

class BravoMaterial : public BravoObject
{
public:
	template <typename... Args>
	BravoMaterial(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	void Load() {};
	bool EnsureReady()
	{
		return ShaderAsset != nullptr && ShaderAsset->EnsureGPUReady();
	}
	
	virtual void Use() = 0;
	virtual void StopUsage() = 0;

	std::shared_ptr<BravoShaderAsset> GetShader() const { return ShaderAsset; }


protected:
	std::shared_ptr<BravoRenderShaderAsset> ShaderAsset = nullptr;
};
