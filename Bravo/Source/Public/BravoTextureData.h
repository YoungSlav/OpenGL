#pragma once
#include "stdafx.h"
#include "stb/stb_image.h"

class BravoTextureData
{
public:
	BravoTextureData(const std::string& _Path, bool sRGB = false)
	{
		int32 nrChannels;
		if ( TextureData = stbi_loadf(_Path.c_str(), &SizeX, &SizeY, &nrChannels, 0) )
		{
			if (nrChannels == 1)
			{
				TextureFormat = GL_RED;
				TextureInnerFormat = GL_R32F;
			}
			else if (nrChannels == 3)
			{
				TextureInnerFormat = sRGB ? GL_SRGB8 : GL_RGB32F;
				TextureFormat = GL_RGB;
			}
			else if (nrChannels == 4)
			{
				TextureInnerFormat = sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA32F;
				TextureFormat = GL_RGBA;
			}
			else
			{
				stbi_image_free(TextureData);
				return;
			}

			bInitialized = true;
		}
	}
	~BravoTextureData()
	{
		if ( TextureData )
			stbi_image_free(TextureData);
	}


	bool bInitialized = false;

	int32 SizeX = 0;
	int32 SizeY = 0;
	GLenum TextureFormat = GL_RED;
	GLenum TextureInnerFormat = GL_R32F;
	float* TextureData = nullptr;
};

