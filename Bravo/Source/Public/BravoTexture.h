#pragma once

#include "stdafx.h"
#include "BravoAsset.h"




class BravoTexture : public BravoAsset
{

	

public:

	virtual void Use() override;
	virtual void StopUsage() override;

	inline const int GetSizeX() const { return SizeX; }
	inline const int GetSizeY() const { return SizeY; }
	inline const int GetTextureUnit() const { return TextureUnit; }

protected:
	virtual bool Load_Internal();
	virtual void UnLoad_Internal();


	int SizeX;
	int SizeY;

private:
	int TextureUnit = -1;
};

typedef std::shared_ptr<BravoTexture> BravoTexturePtr;



