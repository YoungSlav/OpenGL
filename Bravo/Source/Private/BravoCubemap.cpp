#include "BravoCubemap.h"
#include "BravoTextureUnitManager.h"
#include "BravoTextureData.h"
#include "openGL.h"
#include "stdafx.h"

BravoCubemap::~BravoCubemap()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	glDeleteTextures(1, &TextureID);
	TextureID = 0;
}

bool BravoCubemap::Initialize_Internal(const std::vector<std::string>& _Params)
{
	if ( _Params.size() != 6 )
	{
		Log::LogMessage("Invalid number of textures to initialize cubemap: " + Path, ELog::Error);
		return false;
	}
	bool success = true;
	for (unsigned int i = 0; i < 6; i++)
	{
		Textures[i] = std::shared_ptr<BravoTextureData>(new BravoTextureData(Path + _Params[i]));

		success = success && Textures[i] != nullptr && Textures[i]->bInitialized;
	}

	return success;
}

bool BravoCubemap::LoadToGPU_Internal()
{
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

		for (unsigned int i = 0; i < 6; i++)
		{
			if ( Textures[i] && Textures[i]->TextureData )
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Textures[i]->TextureFormat, Textures[i]->SizeX, Textures[i]->SizeY, 0, Textures[i]->TextureFormat, GL_UNSIGNED_BYTE, Textures[i]->TextureData);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return true;
}
void BravoCubemap::ReleaseFromGPU_Internal()
{
	StopUsage();
	glDeleteTextures(1, &TextureID);
}


void BravoCubemap::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = BravoTextureUnitManager::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP,  TextureID);
}

void BravoCubemap::StopUsage()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP,  0);
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	TextureUnit = -1;
}
