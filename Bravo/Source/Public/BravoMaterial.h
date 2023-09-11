#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoShaderAsset.h"
#include "BravoTextureAsset.h"

class BravoMaterial : public BravoObject
{
public:
	void Load() {};
	bool EnsureReady()
	{
		return ShaderAsset != nullptr && ShaderAsset->EnsureReady();
	}
	
	virtual void Use() = 0;
	virtual void StopUsage() = 0;

	std::shared_ptr<BravoShaderAsset> GetShader() const { return ShaderAsset; }


protected:
	std::shared_ptr<BravoShaderAsset> ShaderAsset = nullptr;
};
