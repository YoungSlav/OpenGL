#pragma once
#include "stdafx.h"
#include "stb/stb_image.h"

class BravoTextureData
{
public:
	BravoTextureData(const std::string& _Path, bool bFloat = false, bool sRGB = false)
	{
		if ( sRGB ) bFloat = false;

		TextureData = bFloat ?
			(void*)stbi_loadf(_Path.c_str(), &SizeX, &SizeY, &nrChannels, 0) :
			(void*)stbi_load_16(_Path.c_str(), &SizeX, &SizeY, &nrChannels, 0);
		if ( TextureData )
		{
			if (nrChannels == 1)
			{
				TextureFormat = GL_RED;
				TextureInnerFormat = bFloat ? GL_R32F : GL_R16;
			}
			else if (nrChannels == 3)
			{
				TextureInnerFormat = sRGB ? GL_SRGB8 : bFloat ? GL_RGB32F : GL_RGB16;
				TextureFormat = GL_RGB;
			}
			else if (nrChannels == 4)
			{
				TextureInnerFormat = sRGB ? GL_SRGB8_ALPHA8 : bFloat ? GL_RGBA32F : GL_RGBA16;
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

	int32 nrChannels = 0;
	int32 SizeX = 0;
	int32 SizeY = 0;
	GLenum TextureFormat = GL_RED;
	GLenum TextureInnerFormat = GL_R32F;
	void* TextureData = nullptr;
};

