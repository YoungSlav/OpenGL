#pragma once

#include "stdafx.h"
#include "BravoAsset.h"


struct BravoTextureLoadingParams
{
	BravoTextureLoadingParams(const std::string& _Path, bool _bFloat = false, bool _sRGB = false) :
		TexturePath(_Path),
		bFloat(_bFloat),
		sRGB(_sRGB)
	{}

	std::string TexturePath;
	bool sRGB = false;
	bool bFloat = false;
};

class BravoTextureAsset : public BravoAsset
{
public:
	template <typename... Args>
	BravoTextureAsset(Args&&... args) :
		BravoAsset(std::forward<Args>(args)...)
	{}


	EAssetLoadingState Load(const BravoTextureLoadingParams& params);

	virtual void Use() override;
	virtual void StopUsage() override;

	inline const int32 GetTextureUnit() const { return TextureUnit; }
	inline std::shared_ptr<class BravoTextureData> GetTextureData() const { return TextureData; }

protected:

	void AsyncLoad(const BravoTextureLoadingParams& params);

	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;


private:
	std::shared_ptr<class BravoTextureData> TextureData;

	GLuint TextureID = 0;
	int32 TextureUnit = -1;
};



