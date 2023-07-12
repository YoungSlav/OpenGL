#pragma once
#include "BravoActor.h"
#include "BravoMesh.h"
#include "BravoShader.h"
#include "BravoShadowMap.h"
#include "BravoRenderable.h"

struct BravoLightColor
{
	BravoLightColor() = default;
	BravoLightColor(const glm::vec3& SimpleColor) :
		ambient(SimpleColor*0.2f), diffuse(SimpleColor*0.5f), specular(SimpleColor*0.1f)
	{}
	BravoLightColor( const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular)
	{}

	glm::vec3 ambient = glm::vec3(0.05f);
	glm::vec3 diffuse = glm::vec3(0.5f);
	glm::vec3 specular = glm::vec3(0.1f);
};

class BravoLightActor : public BravoActor
{
public:
	BravoLightActor() = default;

	void SetLightColor(const BravoLightColor& InLightColor) { LightColor = InLightColor; }
	virtual void Use(BravoShaderPtr OnShader);
	virtual void StopUsage();

	void SetShaderPath(const std::string& InPath) { Path = InPath; }
	virtual void UpdateShadowMap();

protected:
	virtual void OnDestroy() override;

protected:
	std::string Path = "";
	BravoLightColor LightColor;

	std::shared_ptr<BravoShadowMap> ShadowMap;
};

class BravoDirLightActor : public BravoLightActor
{
public:

	virtual void Use(BravoShaderPtr OnShader) override;

protected:
	virtual bool Initialize_Internal() override;
};

class BravoPointLightActor : public BravoLightActor
{
public:

	virtual void Use(BravoShaderPtr OnShader) override;

protected:
	virtual bool Initialize_Internal() override;
};

class BravoSpotLightActor : public BravoLightActor
{
public:

	virtual void Use(BravoShaderPtr OnShader) override;

	float CutOff;
	float OuterCutOff;
	float Constant;
	float Linear;
	float Quadratic;

protected:
	virtual bool Initialize_Internal() override;
};