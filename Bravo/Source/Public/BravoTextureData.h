#pragma once
#include "stdafx.h"
#include "stb_image.h"

class BravoTextureData
{
public:
	BravoTextureData(const std::string& _Path)
	{
		int nrChannels;
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
				Log::LogMessage("Failed to load texture \"" + _Path + "\": unknown texture format", ELog::Error);
				return;
			}

			bInitialized = true;
		}
		else
		{
			Log::LogMessage("Failed to load texture \"" + _Path + "\"", ELog::Error);
		}
	}
	~BravoTextureData()
	{
		if ( TextureData )
			stbi_image_free(TextureData);
	}


	bool bInitialized = false;

	int SizeX = -1;
	int SizeY = -1;
	GLenum TextureFormat = 0;
	unsigned char* TextureData = nullptr;
};

