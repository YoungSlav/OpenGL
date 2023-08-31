#pragma once

#include "stdafx.h"
#include "BravoAsset.h"


struct BravoTextureLoadingParams
{
	BravoTextureLoadingParams(const std::string& _Path) :
		TexturePath(_Path)
	{}

	std::string TexturePath;
};

class BravoTextureAsset : public BravoAsset
{
public:
	bool Load(const std::string& ResourcesPath, const BravoTextureLoadingParams& params);

	virtual void Use() override;
	virtual void StopUsage() override;

	inline const int32 GetTextureUnit() const { return TextureUnit; }
	inline std::shared_ptr<class BravoTextureData> GetTextureData() const { return TextureData; }

protected:
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;


private:
	std::shared_ptr<class BravoTextureData> TextureData;

	GLuint TextureID = 0;
	int32 TextureUnit = -1;
};


