#pragma once
#include "BravoPlayer.h"
#include "BravoShader.h"

class BravoShadowMap : public BravoObject
{
public:

	virtual void Setup(const glm::ivec2& Size) = 0;
	virtual void Render(std::shared_ptr<class BravoLightActor> Owner) = 0;

	virtual void Use(BravoShaderPtr OnShader, const std::string& Path) = 0;
	virtual void StopUsage() = 0;

protected:
	virtual void OnDestroy() override;

	glm::ivec2 Size;
	std::weak_ptr<class BravoShader> Shader;
	std::shared_ptr<class BravoShader> GetShader() const { return Shader.expired() ? nullptr : Shader.lock(); }
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
	
	int32 TextureUnit = 0;
	uint32 DepthMapFBO = 0;
	uint32 DepthMap = 0;
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

	int32 TextureUnit = 0;
	uint32 DepthMapFBO = 0;
	uint32 DepthCubemap = 0;
};

class BravoShadowMap_Directional : public BravoShadowMap_Texture
{
public:

	virtual void Render(std::shared_ptr<class BravoLightActor> Owner) override;
};

class BravoShadowMap_Spot : public BravoShadowMap_Texture
{
public:

	virtual void Render(std::shared_ptr<class BravoLightActor> Owner) override;
};

class BravoShadowMap_Point : public BravoShadowMap_Cube
{
public:

	virtual void Render(std::shared_ptr<class BravoLightActor> Owner) override;
};
