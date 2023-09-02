#pragma once
#include "BravoMaterial.h"

struct BravoUnlitMaterialParams
{
	std::string AlbedoTexture = "";
	glm::vec3 AlbedoColor = glm::vec3(0.0f);
};

class BravoMaterialUnlit : public BravoMaterial
{
public:
	void Load(const BravoUnlitMaterialParams& params);
	
	virtual void Use() override;
	virtual void StopUsage() override;

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

protected:
	std::shared_ptr<class BravoTextureAsset> AlbedoTexture = nullptr;

	glm::vec3 AlbedoColor = glm::vec3(0.0f);
};

