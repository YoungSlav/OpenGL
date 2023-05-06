#include "stdafx.h"
#include "BravoTextureUnitManager.h"
#include "openGL.h"

BravoTextureUnitManager::BravoTextureUnitManager()
{
	int TextureUnitCount = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &(TextureUnitCount));
	TextureUnits.resize(TextureUnitCount, false);
}

int BravoTextureUnitManager::BindTexture()
{
	int EmptyUnit = Instance().FindEmptyTextureUnit();
	
	if ( EmptyUnit < 0 || EmptyUnit >= (int)Instance().TextureUnits.size() )
		return -1;
	
	return EmptyUnit;
}

void BravoTextureUnitManager::UnbindTexture(int TextureUnit)
{
	if ( TextureUnit < 0 || TextureUnit >= (int)Instance().TextureUnits.size() )
		return;
	Instance().TextureUnits[TextureUnit] = false;
}

int BravoTextureUnitManager::FindEmptyTextureUnit()
{
	for ( unsigned int i = 0; i < TextureUnits.size(); ++i )
	{
		if ( !TextureUnits[i] )
		{
			TextureUnits[i] = true;
			return i;
		}
	}

	Log::LogMessage("No empty texture units", ELog::Warning);
	return -1;
}