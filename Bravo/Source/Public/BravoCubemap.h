#pragma once
#include "BravoAsset.h"

class BravoCubemap : public BravoAsset
{
public:
	BravoCubemap(std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		BravoAsset(_AssetManager)
	{}

	~BravoCubemap();

	virtual void Use() override;
	virtual void StopUsage() override;

	inline const int GetTextureUnit() const { return TextureUnit; }

protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) override;
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;

	
	std::shared_ptr<class BravoTextureData> Textures[6];

private:
	unsigned int TextureID = 0;
	int TextureUnit = -1;
};

typedef std::shared_ptr<BravoCubemap> BravoCubemapPtr;
