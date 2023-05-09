#pragma once

#include "stdafx.h"
#include "BravoAsset.h"

#include "stb_truetype.h"

class BravoFont : public BravoAsset
{
	struct FontInfo
	{
		FontInfo() = default;

		FontInfo(uint32 GlyphsNum) :
			Glyphs(GlyphsNum),
			Ascent(0.0f),
			Descent(0.0f),
			Linegap(0.0f)
		{}

		std::vector<stbtt_packedchar> Glyphs;
		float Ascent = 0.0f;
		float Descent = 0.0f;
		float Linegap = 0.0f;
	};

public:
	BravoFont(std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		BravoAsset(_AssetManager)
	{}
	~BravoFont() {}
	

	//virtual void Use() override;
	//virtual void StopUsage() override;


protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) override;
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;

private:

	GLuint TextureID = 0;
	int32 TextureUnit = -1;

	// size -> glyphs collection
	std::map<uint32, FontInfo> Fonts;

	std::vector<uint8> AtlasBitmap;

	const int32 AtlasSizeX = 1024;
	const int32 AtlasSizeY = 2048;

	const int32 FirstGlyph = 32;
	const int32 GlyphsNum = 95;
};

typedef std::shared_ptr<BravoFont> BravoFontPtr;