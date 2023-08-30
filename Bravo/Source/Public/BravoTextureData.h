#pragma once
#include "stdafx.h"
#include "stb_image.h"

class BravoTextureData
{
public:
	BravoTextureData(const std::string& _Path)
	{
		int32 nrChannels;

		if ( TextureData = stbi_load(_Path.c_str(), &SizeX, &SizeY, &nrChannels, 0) )
		{
			if (nrChannels == 1)
				TextureFormat = GL_RED;
			else if (nrChannels == 3)
				TextureFormat = GL_RGB;
			else if (nrChannels == 4)
				TextureFormat = GL_RGBA;
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
	uint8* TextureData = nullptr;
};

