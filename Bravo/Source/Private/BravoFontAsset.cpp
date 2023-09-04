#include "BravoFontAsset.h"
#include "openGL.h"
#include <fstream>
#include "stb_rect_pack.h"
#include "BravoTextureUnitManager.h"
#include <math.h>

#define POINT_TO_PIXEL 1.333f

EAssetLoadingState BravoFontAsset::Load(const std::string& ResourcesPath, const BravoFontLoadingParams& params)
{
	if ( !params.FontSizes.size() )
	{
		Log::LogMessage("Failed to load font " + GetName() +". Expecting sizes array in params.", ELog::Error );
		LoadingState = EAssetLoadingState::Failed;
		return LoadingState;
	}

	std::string Path = ResourcesPath + params.FontPath;

	std::ifstream file(&*Path.begin(), std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<int8> fontBuffer((uint32)size);
	if ( !file.read(fontBuffer.data(), size) )
	{
		Log::LogMessage("Failed to read font from file " + Path, ELog::Error);
		LoadingState = EAssetLoadingState::Failed;
		return LoadingState;
	}
	stbtt_fontinfo fontInfo;
    stbtt_InitFont(&fontInfo, reinterpret_cast<const uint8*>(fontBuffer.data()), stbtt_GetFontOffsetForIndex(reinterpret_cast<const uint8*>(fontBuffer.data()),0));

	for ( uint32 fSize : params.FontSizes )
	{
		Fonts.insert({fSize, BravoFontInfo(GlyphsNum, fSize)});
	}

	std::vector<stbtt_pack_range> ranges;
	ranges.reserve(params.FontSizes.size());
	for ( uint32 fSize : params.FontSizes )
	{
		float fontSize = (float)fSize * POINT_TO_PIXEL;
		stbtt_pack_range newRange { fontSize, FirstGlyph, NULL, GlyphsNum, Fonts[fSize].Glyphs.data(), 0, 0 };
		ranges.push_back(newRange);
	}

	AtlasBitmap.resize(AtlasSizeY*AtlasSizeX);

	  // do the packing, based on the ranges specified
    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, AtlasBitmap.data(), AtlasSizeX, AtlasSizeY, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 3, 3); // say, choose 3x1 oversampling for subpixel positioning
    if ( !stbtt_PackFontRanges(&pc, reinterpret_cast<const uint8*>(fontBuffer.data()), 0, ranges.data(), (int32)ranges.size()) )
	{
		Log::LogMessage("Failed to load font " + Path +". Atlas size is too small", ELog::Error );
		stbtt_PackEnd(&pc);
		AtlasBitmap.clear();
		LoadingState = EAssetLoadingState::Failed;
		return LoadingState;
	}
    stbtt_PackEnd(&pc);

	for ( auto& font : Fonts )
	{
		float scale = stbtt_ScaleForPixelHeight(&fontInfo, (float)font.first);
		int32 a, d, l;
		stbtt_GetFontVMetrics(&fontInfo, &a, &d, &l);
		
		font.second.Ascent  = a*scale;
		font.second.Descent = d*scale;
		font.second.Linegap = l*scale;
	}
	LoadingState = EAssetLoadingState::InRAM;
	return LoadingState;
}

bool BravoFontAsset::LoadToGPU_Internal()
{
	if ( AtlasBitmap.size() != AtlasSizeX*AtlasSizeY )
	{
		Log::LogMessage("Failed to load font " + GetName() +" to GPU. Atlas is invalid.", ELog::Error );
		return false;
	}
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, AtlasSizeX, AtlasSizeY, 0, GL_RED, GL_UNSIGNED_BYTE, AtlasBitmap.data() );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D,  0);

	return true;
}

void BravoFontAsset::ReleaseFromGPU_Internal()
{
	StopUsage();
	glDeleteTextures(1, &TextureID);
	TextureID = 0;
}


void BravoFontAsset::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = BravoTextureUnitManager::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  TextureID);
}


void BravoFontAsset::StopUsage()
{
	//glBindTexture(GL_TEXTURE_2D,  0);
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	TextureUnit = -1;
}

const BravoFontInfo* BravoFontAsset::GetFontInfo( uint32 TextSize, float TargetScale ) const
{
	return FindBestFont((uint32)((float)(TextSize)*TargetScale));
}

float BravoFontAsset::GetCharacterQuad(char symbol, uint32 TextSize, float TargetScale, float xpos, float ypos, bool bAlignToInteger, stbtt_aligned_quad* OutQuad) const
{
	const BravoFontInfo* BestFont = FindBestFont((uint32)((float)(TextSize)*TargetScale));
	if ( BestFont == nullptr )
		return 0.0f;
		
	const int32 CharIndex = (int32)(symbol) - FirstGlyph;
	if ( CharIndex < 0 || (uint32)CharIndex >= BestFont->Glyphs.size() )
		return 0.0f;
		
	float Scale = 1.0f;
	if ( TextSize != BestFont->Size )
	{
		Scale = (float)(TextSize) / (float)(BestFont->Size);
	}
	Scale *= TargetScale;

	const stbtt_packedchar *b = &BestFont->Glyphs[CharIndex];

	if (bAlignToInteger)
	{
		float x = (float) ((int32) floor((xpos + b->xoff*Scale) + 0.5f));
		float y = (float) ((int32) floor((ypos + b->yoff*Scale) + 0.5f));
		OutQuad->x0 = x;
		OutQuad->y0 = y;
		OutQuad->x1 = x + b->xoff2*Scale - b->xoff*Scale;
		OutQuad->y1 = y + b->yoff2*Scale - b->yoff*Scale;
	}
	else
	{
		OutQuad->x0 = xpos + b->xoff*Scale;
		OutQuad->y0 = ypos + b->yoff*Scale;
		OutQuad->x1 = xpos + b->xoff2*Scale;
		OutQuad->y1 = ypos + b->yoff2*Scale;
	}

	float ipw = 1.0f / (float)AtlasSizeX;
	float iph = 1.0f / (float)AtlasSizeY;

	OutQuad->s0 = b->x0 * ipw;
	OutQuad->t0 = b->y0 * iph;
	OutQuad->s1 = b->x1 * ipw;
	OutQuad->t1 = b->y1 * iph;

	return b->xadvance*Scale;
}

const BravoFontInfo* BravoFontAsset::FindBestFont(uint32 TextSize) const
{
	if ( Fonts.empty() )
		return nullptr;

	auto it = Fonts.lower_bound(TextSize);
	if ( it == Fonts.end() )
		it--;
	return &it->second;
}