#include "stdafx.h"
#include "BravoTextureUnitManager.h"
#include "openGL.h"

BravoTextureUnitManager::BravoTextureUnitManager()
{
	int32 TextureUnitCount = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &(TextureUnitCount));
	TextureUnits.resize(TextureUnitCount, false);
}

int32 BravoTextureUnitManager::BindTexture()
{
	int32 EmptyUnit = Instance().FindEmptyTextureUnit();
	
	if ( EmptyUnit < 0 || EmptyUnit >= (int32)Instance().TextureUnits.size() )
		return -1;
	
	return EmptyUnit;
}

void BravoTextureUnitManager::UnbindTexture(int32 TextureUnit)
{
	if ( TextureUnit < 0 || TextureUnit >= (int32)Instance().TextureUnits.size() )
		return;
	Instance().TextureUnits[TextureUnit] = false;
}

int32 BravoTextureUnitManager::FindEmptyTextureUnit()
{
	for ( uint32 i = 0; i < TextureUnits.size(); ++i )
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