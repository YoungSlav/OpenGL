#include "BravoTexture.h"
#include "BravoTextureData.h"
#include "BravoTextureUnitManager.h"
#include "openGL.h"
#include "stdafx.h"

BravoTexture::~BravoTexture()
{
	if ( TextureID )
	{
		glDeleteTextures(1, &TextureID);
		TextureID = 0;
	}

	BravoTextureUnitManager::UnbindTexture(TextureUnit);
}

bool BravoTexture::Initialize_Internal(const std::vector<std::string>& _Params)
{
	TextureData = std::shared_ptr<BravoTextureData>(new BravoTextureData(Path));
	return TextureData != nullptr && TextureData->bInitialized;
}

bool BravoTexture::LoadToGPU_Internal()
{
	if (TextureData->TextureData)
	{
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D,  TextureID);
			// set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, TextureData->TextureFormat, TextureData->SizeX, TextureData->SizeY, 0, TextureData->TextureFormat, GL_UNSIGNED_BYTE, TextureData->TextureData);
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D,  0);

		return true;
	}
	else
	{
		Log::LogMessage("Failed to load texture \"" + Path + "\" to GPU", ELog::Error);
		return false;
	}
}
void BravoTexture::ReleaseFromGPU_Internal()
{
	StopUsage();
	glDeleteTextures(1, &TextureID);
	TextureID = 0;
}

void BravoTexture::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = BravoTextureUnitManager::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  TextureID);
}

void BravoTexture::StopUsage()
{
	glBindTexture(GL_TEXTURE_2D,  0);
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	TextureUnit = -1;
}

