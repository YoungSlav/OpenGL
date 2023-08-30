#include "BravoMaterialAsset.h"
#include "BravoTextureUnitManager.h"
#include "BravoTextureData.h"
#include "openGL.h"


bool BravoMaterialAsset::Load(const std::string& ResourcesPath, const BravoMaterialLoadingParams& params)
{
	albedoTextureData = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.AlbedoTexture));
	metallicTextureData = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.MetallicTexture));
	roughnessTextureData = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.RoughnessTexture));
	aoTextureData = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.AoTexture));
	normalTextureData = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.NormalTexture));

	ShaderData.useAlbedoTexture = albedoTextureData != nullptr && albedoTextureData->bInitialized;
	ShaderData.useMetallicTexture = metallicTextureData != nullptr && metallicTextureData->bInitialized;
	ShaderData.useRoughnessTexture = roughnessTextureData != nullptr && roughnessTextureData->bInitialized;
	ShaderData.useAoTexture = aoTextureData != nullptr && aoTextureData->bInitialized;
	ShaderData.useNormalTexture = normalTextureData != nullptr && normalTextureData->bInitialized;

	ShaderData.albedoColor = params.AlbedoColor;
	ShaderData.metallicColor = params.MetallicColor;
	ShaderData.roughnessColor = params.RoughnessColor;
	ShaderData.aoColor = params.AoColor;

	return true;
}

bool BravoMaterialAsset::LoadToGPU_Internal()
{
	ShaderData.albedoTexture = LoadTextureToGPU(albedoTextureData);
	ShaderData.metallicTexture = LoadTextureToGPU(metallicTextureData);
	ShaderData.roughnessTexture = LoadTextureToGPU(roughnessTextureData);
	ShaderData.aoTexture = LoadTextureToGPU(aoTextureData);
	ShaderData.normalTexture = LoadTextureToGPU(normalTextureData);

	return true;
}

GLuint BravoMaterialAsset::LoadTextureToGPU(std::shared_ptr<BravoTextureData> TextureData)
{
	GLuint TextureID;
	if ( !TextureData->bInitialized || !TextureData->TextureData )
	{
		return 0;
	}

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D,  TextureID);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, TextureData->TextureFormat, TextureData->SizeX, TextureData->SizeY, 0, TextureData->TextureFormat, GL_UNSIGNED_BYTE, TextureData->TextureData);
		glGenerateMipmap(GL_TEXTURE_2D);

	return TextureID;
}

void BravoMaterialAsset::ReleaseFromGPU_Internal()
{
	StopUsage();
	glDeleteTextures(1, &(ShaderData.albedoTexture));
	glDeleteTextures(1, &(ShaderData.metallicTexture));
	glDeleteTextures(1, &(ShaderData.roughnessTexture));
	glDeleteTextures(1, &(ShaderData.aoTexture));
	glDeleteTextures(1, &(ShaderData.normalTexture));
}

void BravoMaterialAsset::Use()
{
	if ( ShaderData.useAlbedoTexture && ShaderData.albedoTexture != 0 )
	{
		ShaderData.albedoTextureUnit = BravoTextureUnitManager::BindTexture();
		glActiveTexture(GL_TEXTURE0 + ShaderData.albedoTextureUnit);
		glBindTexture(GL_TEXTURE_2D,  ShaderData.albedoTexture);
	}
	
	if ( ShaderData.useMetallicTexture && ShaderData.metallicTexture != 0 )
	{
		ShaderData.metallicTextureUnit = BravoTextureUnitManager::BindTexture();
		glActiveTexture(GL_TEXTURE0 + ShaderData.metallicTextureUnit);
		glBindTexture(GL_TEXTURE_2D,  ShaderData.metallicTexture);
	}
	
	if ( ShaderData.useRoughnessTexture && ShaderData.roughnessTexture != 0 )
	{
		ShaderData.roughnessTextureUnit = BravoTextureUnitManager::BindTexture();
		glActiveTexture(GL_TEXTURE0 + ShaderData.roughnessTextureUnit);
		glBindTexture(GL_TEXTURE_2D,  ShaderData.roughnessTexture);
	}
	
	if ( ShaderData.useAoTexture && ShaderData.aoTexture != 0 )
	{
		ShaderData.aoTextureUnit = BravoTextureUnitManager::BindTexture();
		glActiveTexture(GL_TEXTURE0 + ShaderData.useAoTexture);
		glBindTexture(GL_TEXTURE_2D,  ShaderData.aoTexture);
	}
	
	if ( ShaderData.useNormalTexture && ShaderData.normalTexture != 0 )
	{
		ShaderData.normalTextureUnit = BravoTextureUnitManager::BindTexture();
		glActiveTexture(GL_TEXTURE0 + ShaderData.useNormalTexture);
		glBindTexture(GL_TEXTURE_2D,  ShaderData.normalTexture);
	}

}

void BravoMaterialAsset::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(ShaderData.albedoTextureUnit);
	BravoTextureUnitManager::UnbindTexture(ShaderData.metallicTextureUnit);
	BravoTextureUnitManager::UnbindTexture(ShaderData.roughnessTextureUnit);
	BravoTextureUnitManager::UnbindTexture(ShaderData.aoTextureUnit);
	BravoTextureUnitManager::UnbindTexture(ShaderData.normalTextureUnit);
}