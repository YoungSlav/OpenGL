#pragma once

#include "stdafx.h"
#include "BravoAsset.h"



class BravoTexture : public BravoAsset
{
public:
	BravoTexture(std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		BravoAsset(_AssetManager)
	{}
	~BravoTexture();
	

	virtual void Use() override;
	virtual void StopUsage() override;

	inline const int GetTextureUnit() const { return TextureUnit; }
	inline std::shared_ptr<class BravoTextureData> GetTextureData() const { return TextureData; }

protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) override;
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;


	std::shared_ptr<class BravoTextureData> TextureData;

private:
	unsigned int TextureID = 0;
	int TextureUnit = -1;
};

typedef std::shared_ptr<BravoTexture> BravoTexturePtr;



