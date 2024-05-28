#include "BravoTextureAsset.h"
#include "BravoTextureData.h"
#include "BravoTextureUnitManager.h"
#include "openGL.h"
#include "stdafx.h"


EAssetLoadingState BravoTextureAsset::Load(const std::string& ResourcesPath, const BravoTextureLoadingParams& params)
{
	std::thread asyncLoadThread(&BravoTextureAsset::AsyncLoad, this, ResourcesPath, params);
	//AsyncLoad(ResourcesPath, params);

	LoadingState = EAssetLoadingState::AsyncLoading;
	asyncLoadThread.detach();
	return LoadingState;
}

void BravoTextureAsset::AsyncLoad(const std::string& ResourcesPath, const BravoTextureLoadingParams& params)
{
	TextureData = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.TexturePath, params.bFloat, params.sRGB));
	if ( TextureData != nullptr && TextureData->bInitialized )
		LoadingState = EAssetLoadingState::InRAM;
	else
		LoadingState = EAssetLoadingState::Failed;
}

bool BravoTextureAsset::LoadToGPU_Internal()
{
	if ( !TextureData->TextureData )
	{
		Log::LogMessage(ELog::Error, "Failed to load texture \" {} \" to GPU", GetName());
		return false;
	}

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D,  TextureID);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, TextureData->TextureInnerFormat, TextureData->SizeX, TextureData->SizeY, 0, TextureData->TextureFormat, GL_UNSIGNED_BYTE, TextureData->TextureData);
		glGenerateMipmap(GL_TEXTURE_2D);

	//glBindTexture(GL_TEXTURE_2D,  0);

	return true;
}
void BravoTextureAsset::ReleaseFromGPU_Internal()
{
	StopUsage();
	glDeleteTextures(1, &TextureID);
	TextureID = 0;
}

void BravoTextureAsset::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = BravoTextureUnitManager::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  TextureID);
}

void BravoTextureAsset::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	TextureUnit = -1;
}

