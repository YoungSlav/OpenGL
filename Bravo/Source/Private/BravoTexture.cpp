#include "BravoTexture.h"
#include "stb_image.h"
#include "openGL.h"
#include "stdafx.h"



bool BravoTexture::Load_Internal()
{
	int nrChannels;
	unsigned char *data = stbi_load(Path.c_str(), &SizeX, &SizeY, &nrChannels, 0);
	if (data)
	{
		glGenTextures(1, & AssetHandle);
		glBindTexture(GL_TEXTURE_2D,  AssetHandle);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, SizeX, SizeY, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		Log::LogMessage("Failed to load texture \"" + Path + "\"", ELog::Error);
		return false;
	}
	stbi_image_free(data);
	return true;
}
void BravoTexture::UnLoad_Internal()
{
	StopUsage();
	glDeleteTextures(1, & AssetHandle);
}

void BravoTexture::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = TextureUnitSelector::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  AssetHandle);
}

void BravoTexture::StopUsage()
{
	glBindTexture(GL_TEXTURE_2D,  0);
	TextureUnitSelector::UnbindTexture(TextureUnit);
	TextureUnit = -1;
}

