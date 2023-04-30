#pragma once
#include "BravoAsset.h"

class BravoCubemap : public BravoAsset
{
public:
	virtual void Use() override;
	virtual void StopUsage() override;

	inline const int GetTextureUnit() const { return TextureUnit; }

protected:
	virtual bool Load_Internal();
	virtual void UnLoad_Internal();

private:
	int TextureUnit = -1;
};

typedef std::shared_ptr<BravoCubemap> BravoCubemapPtr;
