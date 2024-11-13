#pragma once
#include "BravoMaterial.h"

struct BravoPBRMaterialParams
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

	std::string HeightTexture = "";
	float HeightScale = 1.0f;
};

class BravoMaterialPBR : public BravoMaterial
{
public:
	template <typename... Args>
	BravoMaterialPBR(Args&&... args) :
		BravoMaterial(std::forward<Args>(args)...)
	{}


	void Load(const BravoPBRMaterialParams& params);
	
	virtual void Use() override;
	virtual void StopUsage() override;

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

protected:
	std::shared_ptr<class BravoTextureAsset> AlbedoTexture = nullptr;
	std::shared_ptr<class BravoTextureAsset> MetallicTexture = nullptr;
	std::shared_ptr<class BravoTextureAsset> RoughnessTexture = nullptr;
	std::shared_ptr<class BravoTextureAsset> AoTexture = nullptr;
	std::shared_ptr<class BravoTextureAsset> NormalTexture = nullptr;
	std::shared_ptr<class BravoTextureAsset> HeightTexture = nullptr;

	glm::vec3 AlbedoColor = glm::vec3(0.0f);
	float MetallicColor = 0.0f;
	float RoughnessColor = 0.0f;
	float AoColor = 0.0f;
	float HeightScale = 1.0f;

};
