#include "BravoCubemap.h"
#include "stb_image.h"
#include "openGL.h"
#include "stdafx.h"

bool BravoCubemap::Load_Internal()
{
	glGenTextures(1, &AssetHandle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, AssetHandle);
		int width, height, nrChannels;


		stbi_set_flip_vertically_on_load(false);
		for (unsigned int i = 0; i < 6; i++)
		{
			std::string texturePath = Path + AdditionalParams[i];
			unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				GLenum format;
				if (nrChannels == 1)
					format = GL_RED;
				else if (nrChannels == 3)
					format = GL_RGB;
				else if (nrChannels == 4)
					format = GL_RGBA;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				Log::LogMessage("Cubemap texture failed to load at path: "  + AdditionalParams[i], ELog::Error);
				stbi_image_free(data);

				glDeleteTextures(1, &AssetHandle);
				return false;
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return true;
}
void BravoCubemap::UnLoad_Internal()
{
	StopUsage();
	glDeleteTextures(1, &AssetHandle);
}


void BravoCubemap::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = TextureUnitSelector::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP,  AssetHandle);
}
void BravoCubemap::StopUsage()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP,  0);
	TextureUnitSelector::UnbindTexture(TextureUnit);
	TextureUnit = -1;
}
