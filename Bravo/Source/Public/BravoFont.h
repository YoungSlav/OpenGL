#pragma once

#include "stdafx.h"
#include "BravoAsset.h"
#include "stb_truetype.h"

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

class BravoFont : public BravoAsset
{
public:
	BravoFont(std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		BravoAsset(_AssetManager)
	{}
	~BravoFont() {}
	

	virtual void Use() override;
	void SetModelMatrix(const glm::mat4& _matrix);
	void SetColor(const glm::vec4& _color);
	virtual void StopUsage() override;


	float GetCharacterQuad(char symbol, uint32 TextSize, float TargetScale, float xpos, float ypos, bool bAlignToInteger, stbtt_aligned_quad* OutQuad) const;

	const BravoFontInfo* GetFontInfo( uint32 TextSize, float TargetScale ) const;

protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) override;
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;

	const BravoFontInfo* FindBestFont(uint32 TextSize) const;

	GLuint TextureID = 0;
	int32 TextureUnit = -1;

	// size -> glyphs collection
	std::map<uint32, BravoFontInfo> Fonts;

	std::vector<uint8> AtlasBitmap;

	const int32 AtlasSizeX = 4096;
	const int32 AtlasSizeY = 4096;

	const int32 FirstGlyph = 32;
	const int32 GlyphsNum = 96;
};

typedef std::shared_ptr<BravoFont> BravoFontPtr;