#pragma once
#include "BravoActor.h"
#include "BravoMesh.h"
#include "BravoShader.h"
#include "BravoShadowMap.h"

class BravoLightActor : public BravoActor, public BravoTickable
{
public:
	void SetLightColor(const glm::vec3& InLightColor) { LightColor = InLightColor; }
	virtual void Use(BravoShaderPtr OnShader);
	virtual void StopUsage();

	void SetShaderPath(const std::string& InPath) { Path = InPath; }

	virtual void UpdateShadowMap();

protected:
	virtual void Init() override;
	virtual void Draw(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	
	BravoShadowMap* ShadowMap;
	BravoMeshPtr Mesh;
	BravoShaderPtr Shader;

	glm::vec3 LightColor = glm::vec3(1.0f);

	std::string Path = "";
};

class BravoDirLightActor : public BravoLightActor
{
public:
	virtual void Use(BravoShaderPtr OnShader) override;
	virtual void StopUsage() override;

protected:
	virtual void Init() override;
	virtual void OnDestroy() override;
};

class BravoPointLightActor : public BravoLightActor
{
public:
	virtual void Use(BravoShaderPtr OnShader) override;
	virtual void StopUsage() override;

protected:
	virtual void Init() override;
	virtual void OnDestroy() override;


};

class BravoSpotLightActor : public BravoLightActor
{
public:
	virtual void Use(BravoShaderPtr OnShader) override;
	virtual void StopUsage() override;

	float CutOff;
	float OuterCutOff;
	float Constant;
	float Linear;
	float Quadratic;

protected:
	virtual void Init() override;
	virtual void OnDestroy() override;

};