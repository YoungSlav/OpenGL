#pragma once
#include "stdafx.h"
#include "openGL.h"
#include "BravoAsset.h"

// TODO: replace atlas with individual textures and remake for PBR


struct BravoMaterialLoadingParams
{
	std::string DiffuseTexture = "";
	glm::vec3 DiffuseColor = glm::vec3(0);
	
	std::string SpecularTexture = "";
	glm::vec3 SpecularColor = glm::vec3(0);
	
	std::string NormalTexture = "";

	float Shininess = 0.0f;
};

struct BravoMaterialShaderData
{
	int32 DiffuseTextureLayer = -1;
	glm::vec3 DiffuseColor = glm::vec3(0);
	
	int32 SpecularTextureLayer = -1;
	glm::vec3 SpecularColor = glm::vec3(0);
	
	int32 NormalTextureLayer = -1;

	float Shininess = 0.0f;
};

class BravoMaterialAsset : public BravoAsset
{
public:
	bool Load(const std::string& ResourcesPath, const BravoMaterialLoadingParams& params);

	virtual void Use() override;
	virtual void StopUsage() override;

	inline const int32 GetTextureUnit() const { return TextureUnit; }
	inline const BravoMaterialShaderData& GetShadeData() const { return ShaderData; }

protected:
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;


protected:

	BravoMaterialShaderData ShaderData;

	std::vector<std::shared_ptr<class BravoTextureData>> Textures;
	GLuint TextureID = 0;
	int32 TextureUnit = -1;
};
