#pragma once
#include "stdafx.h"
#include "openGL.h"
#include "BravoAsset.h"


struct BravoMaterialLoadingParams
{
	std::string AlbedoTexture = "";
	glm::vec3 AlbedoColor = glm::vec3(0.0f);
	
	std::string MetallicTexture = "";
	float MetallicColor = 0.0f;

	std::string RoughnessTexture = "";
	float RoughnessColor = 0.0f;

	std::string AoTexture = "";
	float AoColor = 0.0f;

	std::string NormalTexture = "";
};

struct BravoMaterialShaderData
{
	GLuint albedoTexture = 0;
	int32 albedoTextureUnit = -1;
	bool useAlbedoTexture = false;
	glm::vec3 albedoColor = glm::vec3(0.0f);
	
	GLuint metallicTexture = 0;
	int32 metallicTextureUnit = -1;
	bool useMetallicTexture = false;
	float metallicColor = 0.0f;

	GLuint roughnessTexture = 0;
	int32 roughnessTextureUnit = -1;
	bool useRoughnessTexture = false;
	float roughnessColor = 0.0f;

	GLuint aoTexture = 0;
	int32 aoTextureUnit = -1;
	bool useAoTexture = false;
	float aoColor = 0.0f;

	GLuint normalTexture = 0;
	int32 normalTextureUnit = -1;
	bool useNormalTexture = false;
};

class BravoMaterialAsset : public BravoAsset
{
public:
	bool Load(const std::string& ResourcesPath, const BravoMaterialLoadingParams& params);

	virtual void Use() override;
	virtual void StopUsage() override;

	inline const BravoMaterialShaderData& GetShadeData() const { return ShaderData; }

protected:
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;


private:
	GLuint LoadTextureToGPU(std::shared_ptr<class BravoTextureData> TextureData);

protected:



	BravoMaterialShaderData ShaderData;

	std::shared_ptr<class BravoTextureData> albedoTextureData = nullptr;
	std::shared_ptr<class BravoTextureData> metallicTextureData = nullptr;
	std::shared_ptr<class BravoTextureData> roughnessTextureData = nullptr;
	std::shared_ptr<class BravoTextureData> aoTextureData = nullptr;
	std::shared_ptr<class BravoTextureData> normalTextureData = nullptr;
};
