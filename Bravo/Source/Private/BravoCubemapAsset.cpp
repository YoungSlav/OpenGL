#include "BravoCubemapAsset.h"
#include "BravoTextureUnitManager.h"
#include "BravoTextureData.h"
#include "openGL.h"
#include "stdafx.h"


EAssetLoadingState BravoCubemapAsset::Load(const std::string& ResourcesPath, const BravoCubemapLoadingParams& params)
{
	std::thread asyncLoadThread(&BravoCubemapAsset::AsyncLoad, this, ResourcesPath, params);
	LoadingState = EAssetLoadingState::AsyncLoading;
	asyncLoadThread.detach();
	return LoadingState;
}

void BravoCubemapAsset::AsyncLoad(const std::string& ResourcesPath, const BravoCubemapLoadingParams& params)
{
	if ( params.TexturesPaths.size() != 6 )
	{
		Log::LogMessage(ELog::Error, "Invalid number of textures to initialize cubemap: {}", GetName() );
		LoadingState = EAssetLoadingState::Failed;
		return;
	}
	bool success = true;
	for (uint32 i = 0; i < 6; i++)
	{
		Textures[i] = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.TexturesPaths[i], false));

		success = success && Textures[i] != nullptr && Textures[i]->bInitialized;
	}
	if ( success )
		LoadingState = EAssetLoadingState::InRAM;
}

bool BravoCubemapAsset::LoadToGPU_Internal()
{
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

		for (uint32 i = 0; i < 6; i++)
		{
			if ( Textures[i] && Textures[i]->TextureData )
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Textures[i]->TextureFormat, Textures[i]->SizeX, Textures[i]->SizeY, 0, Textures[i]->TextureFormat, GL_UNSIGNED_SHORT, Textures[i]->TextureData);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}
void BravoCubemapAsset::ReleaseFromGPU_Internal()
{
	StopUsage();
	glDeleteTextures(1, &TextureID);
}


void BravoCubemapAsset::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = BravoTextureUnitManager::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP,  TextureID);
}

void BravoCubemapAsset::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	TextureUnit = -1;
}
