#include "stdafx.h"
#include "BravoAsset.h"

void BravoAsset::SetPath(const std::string& InPath)
{
	Path = GetRunningDir() + BravoAssetConstants::ResourcesFolderPath + InPath;
}

TextureUnitSelector::TextureUnitSelector()
{
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &(TextureUnitCount));
	TextureUnitsStatus = new int[TextureUnitCount];
	for ( int i = 0; i < TextureUnitCount; ++i )
		TextureUnitsStatus[i] = 0;
}

TextureUnitSelector::~TextureUnitSelector()
{
	delete[] TextureUnitsStatus;
}

int TextureUnitSelector::BindTexture()
{
	int EmptyUnit = Instance().FindEmptyTextureUnit();
	if ( EmptyUnit < 0 || EmptyUnit >= Instance().TextureUnitCount )
		return -1;
	
	return EmptyUnit;
}

void TextureUnitSelector::UnbindTexture(int TextureUnit)
{
	if ( TextureUnit < 0 || TextureUnit >= Instance().TextureUnitCount )
		return;
	Instance().TextureUnitsStatus[TextureUnit] = 0;
}

int TextureUnitSelector::FindEmptyTextureUnit()
{
	for ( int i = 0; i < TextureUnitCount; ++i )
	{
		if ( TextureUnitsStatus[i] == 0 )
		{
			TextureUnitsStatus[i] = 1;
			return i;
		}
	}

	Log::LogMessage("No empty texture units", ELog::Warning);
	return -1;
}
