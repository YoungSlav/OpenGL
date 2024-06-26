#include "stdafx.h"
#include "BravoTextureUnitManager.h"
#include "openGL.h"

BravoTextureUnitManager::BravoTextureUnitManager()
{
	GLint TextureUnitCount = 0;
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

void BravoTextureUnitManager::UnbindTexture(int32& TextureUnit)
{
	if ( TextureUnit < 0 || TextureUnit >= (int32)Instance().TextureUnits.size() )
		return;
	Instance().TextureUnits[TextureUnit] = false;
	TextureUnit = -1;
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

	Log::LogMessage(ELog::Error, "No empty texture units" );
	return -1;
}