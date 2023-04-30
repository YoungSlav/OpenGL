#pragma once
#include "BravoPlayer.h"
#include "BravoShader.h"

class BravoShadowMap : public BravoObject
{
public:
	virtual void Setup(const glm::ivec2& Size) = 0;
	virtual void Draw(class BravoLightActor* Owner) = 0;

	virtual void Use(BravoShaderPtr OnShader, const std::string& Path) = 0;
	virtual void StopUsage() = 0;

protected:
	virtual void OnDestroy() override;

	glm::ivec2 Size;
	std::shared_ptr<class BravoShader> Shader;
};

class BravoShadowMap_Texture : public BravoShadowMap
{
public:
	virtual void Setup(const glm::ivec2& Size) override;

	virtual void Use(BravoShaderPtr OnShader, const std::string& Path) override;
	virtual void StopUsage() override;

protected:
	virtual void OnDestroy() override;

	glm::mat4 LightSpaceMatrix;
	
	int TextureUnit;
	unsigned int DepthMapFBO;
	unsigned int DepthMap;
};

class BravoShadowMap_Cube : public BravoShadowMap
{
public:
	virtual void Setup(const glm::ivec2& Size) override;
	

	virtual void Use(BravoShaderPtr OnShader, const std::string& Path) override;
	virtual void StopUsage() override;

protected:
	virtual void OnDestroy() override;

	std::vector<glm::mat4> ShadowTransforms;

	int TextureUnit;
	unsigned int DepthMapFBO;
	unsigned int DepthCubemap;
};

class BravoShadowMap_Directional : public BravoShadowMap_Texture
{
public:
	virtual void Draw(class BravoLightActor* Owner) override;
};

class BravoShadowMap_Spot : public BravoShadowMap_Texture
{
public:
	virtual void Draw(class BravoLightActor* Owner) override;
};

class BravoShadowMap_Point : public BravoShadowMap_Cube
{
public:
	virtual void Draw(class BravoLightActor* Owner) override;
};
