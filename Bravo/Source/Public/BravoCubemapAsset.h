#pragma once
#include "BravoAsset.h"


struct BravoCubemapLoadingParams
{
	BravoCubemapLoadingParams(const std::vector<std::string>& _Textures) :
		TexturesPaths(_Textures)
	{}

	std::vector<std::string> TexturesPaths;
};

class BravoCubemapAsset : public BravoAsset
{
public:
	EAssetLoadingState Load(const std::string& ResourcesPath, const BravoCubemapLoadingParams& params);

	virtual void Use() override;
	virtual void StopUsage() override;

	inline const int32 GetTextureUnit() const { return TextureUnit; }

protected:

	void AsyncLoad(const std::string& ResourcesPath, const BravoCubemapLoadingParams& params);
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;

	
	std::shared_ptr<class BravoTextureData> Textures[6];

private:
	GLuint TextureID = 0;
	int32 TextureUnit = -1;
};
