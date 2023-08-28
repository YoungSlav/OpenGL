#include "BravoMaterialAsset.h"
#include "BravoTextureUnitManager.h"
#include "BravoTextureData.h"
#include "openGL.h"


bool BravoMaterialAsset::Load(const std::string& ResourcesPath, const BravoMaterialLoadingParams& params)
{
	int32 Layer = 0;
	Textures.reserve(3);
	{
	auto diffuseTexture = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.DiffuseTexture));
	if ( diffuseTexture->bInitialized && diffuseTexture->TextureData )
	{
		Textures.push_back(diffuseTexture);
		ShaderData.DiffuseTextureLayer = Layer;
		Layer++;
	}
	ShaderData.DiffuseColor = params.DiffuseColor;
	}
	{
	auto specularTexture = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.SpecularTexture));
	if ( specularTexture->bInitialized && specularTexture->TextureData )
	{
		Textures.push_back(specularTexture);
		ShaderData.SpecularTextureLayer = Layer;
		Layer++;
	}
	ShaderData.SpecularColor = params.SpecularColor;
	}
	{
	auto normalTexture = std::shared_ptr<BravoTextureData>(new BravoTextureData(ResourcesPath + params.NormalTexture));
	if ( normalTexture->bInitialized && normalTexture->TextureData  )
	{
		Textures.push_back(normalTexture);
		ShaderData.NormalTextureLayer = Layer;
		Layer++;
	}
	}

	ShaderData.Shininess = params.Shininess;

	return true;
}

bool BravoMaterialAsset::LoadToGPU_Internal()
{
	int32 SizeX = Textures.size() ? Textures[0]->SizeX : 0;
	int32 SizeY = Textures.size() ? Textures[0]->SizeY : 0;
	GLenum Format = Textures.size() ? Textures[0]->TextureFormat : GL_RED;

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, TextureID);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA,
			SizeX, SizeY,
			(GLsizei)Textures.size(), 0,
			Format, GL_UNSIGNED_BYTE, nullptr);

		for (size_t i = 0; i < Textures.size(); i++)
		{
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, SizeX, SizeY, 1, Format, GL_UNSIGNED_BYTE, Textures[i]->TextureData);
		}

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}

void BravoMaterialAsset::ReleaseFromGPU_Internal()
{
	StopUsage();
	glDeleteTextures(1, &TextureID);
}

void BravoMaterialAsset::Use()
{
	if ( TextureUnit < 0 )
		TextureUnit = BravoTextureUnitManager::BindTexture();

	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D_ARRAY,  TextureID);
}

void BravoMaterialAsset::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	TextureUnit = -1;
}