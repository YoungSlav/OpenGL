#pragma once

#include "stdafx.h"
#include "BravoAsset.h"
#include "stb/stb_truetype.h"

struct BravoFontInfo
{
	BravoFontInfo() = default;

	BravoFontInfo(uint32 GlyphsNum, uint32 _Size) :
		Glyphs(GlyphsNum),
		Size(_Size)
	{}

	std::vector<stbtt_packedchar> Glyphs;
	uint32 Size = 0;

	float Ascent = 0.0f;
	float Descent = 0.0f;
	float Linegap = 0.0f;
};

struct BravoFontLoadingParams
{
	BravoFontLoadingParams(const std::string& _Path, const std::vector<uint32>& _Sizes) :
		FontPath(_Path),
		FontSizes(_Sizes)
	{}

	std::string FontPath;
	std::vector<uint32> FontSizes;
};

class BravoFontAsset : public BravoAsset
{
public:
	template <typename... Args>
	BravoFontAsset(Args&&... args) :
		BravoAsset(std::forward<Args>(args)...)
	{}


	EAssetLoadingState Load(const BravoFontLoadingParams& params);
	

	virtual void Use() override;
	virtual void StopUsage() override;


	float GetCharacterQuad(char symbol, uint32 TextSize, float TargetScale, float xpos, float ypos, bool bAlignToInteger, stbtt_aligned_quad* OutQuad) const;
	const BravoFontInfo* GetFontInfo( uint32 TextSize, float TargetScale ) const;

protected:
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;

	const BravoFontInfo* FindBestFont(uint32 TextSize) const;

private:
	GLuint TextureID = 0;
	int32 TextureUnit = -1;

	// size -> glyphs collection
	std::map<uint32, BravoFontInfo> Fonts;

	std::vector<uint8> AtlasBitmap;

	const int32 AtlasSizeX = 2048;
	const int32 AtlasSizeY = 2048;

	const int32 FirstGlyph = 32;
	const int32 GlyphsNum = 96;
};