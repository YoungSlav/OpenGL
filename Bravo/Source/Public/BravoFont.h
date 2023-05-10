#pragma once

#include "stdafx.h"
#include "BravoAsset.h"
#include "BravoShader.h"
#include "stb_truetype.h"

struct BravoFontInfo
{
	BravoFontInfo() = default;

	BravoFontInfo(uint32 GlyphsNum) :
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



	// TODO add shader!
	void GetCharacterQuad(char symbol, int32 TextSize, float *xpos, float *ypos, stbtt_aligned_quad* OutQuad, bool align_to_integer) const;


protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) override;
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;

private:

	GLuint TextureID = 0;
	int32 TextureUnit = -1;

	BravoShaderPtr Shader;

	// size -> glyphs collection
	std::map<uint32, BravoFontInfo> Fonts;

	std::vector<uint8> AtlasBitmap;

	const int32 AtlasSizeX = 512;
	const int32 AtlasSizeY = 512;

	const int32 FirstGlyph = 32;
	const int32 GlyphsNum = 96;
};

typedef std::shared_ptr<BravoFont> BravoFontPtr;