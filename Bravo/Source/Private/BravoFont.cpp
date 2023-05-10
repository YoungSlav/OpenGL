#include "BravoFont.h"
#include "openGL.h"
#include <fstream>
#include "stb_rect_pack.h"
#include "BravoTextureUnitManager.h"
#include "BravoAssetManager.h"
#include <math.h>

bool BravoFont::Initialize_Internal(const std::vector<std::string>& _Params)
{
	Shader = GetAssetManager()->LoadAsset<BravoShader>("Shaders\\Text");

	if ( !_Params.size() )
	{
		Log::LogMessage("Failed to load font " + Path +". Expecting sizes array in params.", ELog::Error );
		return false;
	}
	std::vector<uint32> FontSizes;
	FontSizes.reserve(_Params.size());
	for ( auto it : _Params )
		FontSizes.push_back(std::stoi(it));

	std::ifstream file(&*Path.begin(), std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<int8> fontBuffer((uint32)size);
	if ( !file.read(fontBuffer.data(), size) )
	{
		Log::LogMessage("Failed to read font from file " + Path, ELog::Error);
		return false;
	}

	for ( uint32 size : FontSizes )
	{
		Fonts.insert({size, BravoFontInfo(GlyphsNum)});
	}

	std::vector<stbtt_pack_range> ranges;
	ranges.reserve(FontSizes.size());
	for ( uint32 size : FontSizes )
	{
		stbtt_pack_range newRange { (float)size, FirstGlyph, NULL, GlyphsNum, Fonts[size].Glyphs.data(), 0, 0 };
		ranges.push_back(newRange);
	}

	AtlasBitmap.resize(AtlasSizeY*AtlasSizeX);

	  // do the packing, based on the ranges specified
    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, AtlasBitmap.data(), AtlasSizeX, AtlasSizeY, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1); // say, choose 3x1 oversampling for subpixel positioning
    if ( !stbtt_PackFontRanges(&pc, reinterpret_cast<const uint8*>(fontBuffer.data()), 0, ranges.data(), ranges.size()) )
	{
		Log::LogMessage("Failed to load font " + Path +". Atlas size is too small", ELog::Error );
		stbtt_PackEnd(&pc);
		AtlasBitmap.clear();
		return false;
	}
    stbtt_PackEnd(&pc);

	 // get the global metrics for each size/range
    stbtt_fontinfo info;
    stbtt_InitFont(&info, reinterpret_cast<const uint8*>(fontBuffer.data()), stbtt_GetFontOffsetForIndex(reinterpret_cast<const uint8*>(fontBuffer.data()),0));

	for ( uint32 i = 0; i < ranges.size(); ++i )
	{
		float size = ranges[i].font_size;
		float scale = stbtt_ScaleForPixelHeight(&info, ranges[i].font_size);
		int32 a, d, l;
		stbtt_GetFontVMetrics(&info, &a, &d, &l);
		
		Fonts[(uint32)ranges[i].font_size].Ascent  = a*scale;
		Fonts[(uint32)ranges[i].font_size].Descent = d*scale;
		Fonts[(uint32)ranges[i].font_size].Linegap = l*scale;
	}	

	return true;
}

bool BravoFont::LoadToGPU_Internal()
{
	if ( AtlasBitmap.size() != AtlasSizeX*AtlasSizeY )
	{
		Log::LogMessage("Failed to load font " + Path +" to GPU. Atlas is invalid.", ELog::Error );
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
	glBindTexture(GL_TEXTURE_2D,  0);

	return true;
}

void BravoFont::ReleaseFromGPU_Internal()
{
	StopUsage();
	glDeleteTextures(1, &TextureID);
	TextureID = 0;
}


void BravoFont::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = BravoTextureUnitManager::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  TextureID);

	Shader->Use();
}

void BravoFont::SetModelMatrix(const glm::mat4& matrix)
{
	Shader->SetMatrix4d("model", matrix);
}
void BravoFont::SetColor(const glm::vec4& _color)
{
	Shader->SetVector4d("uTextColor", _color);
}

void BravoFont::StopUsage()
{
	glBindTexture(GL_TEXTURE_2D,  0);
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	TextureUnit = -1;

	Shader->StopUsage();
}

void BravoFont::GetCharacterQuad(char symbol, int32 TextSize, float *xpos, float *ypos, stbtt_aligned_quad* OutQuad, bool align_to_integer) const
{
	auto it = Fonts.find(TextSize);
	if ( it == Fonts.end() )
		return;
		
	const int32 CharIndex = (int32)(symbol) - FirstGlyph;
	if ( CharIndex < 0 || (uint32)CharIndex >= it->second.Glyphs.size() )
		return;


	const stbtt_packedchar& CharData = it->second.Glyphs[CharIndex];
	
	stbtt_GetPackedQuad(it->second.Glyphs.data(), AtlasSizeX, AtlasSizeY, CharIndex, xpos, ypos, OutQuad, align_to_integer);
}