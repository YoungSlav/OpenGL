#pragma once
#include "BravoActor.h"
#include "BravoMesh.h"
#include "BravoShader.h"
#include "BravoShadowMap.h"

class BravoLightActor : public BravoActor, public BravoTickable
{
public:
	BravoLightActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoActor(_Engine, _Handle),
		BravoTickable()
	{
		RenderPriority = INT_MIN;
	}

	void SetLightColor(const glm::vec3& InLightColor) { LightColor = InLightColor; }
	virtual void Use(BravoShaderPtr OnShader);
	virtual void StopUsage();

	void SetShaderPath(const std::string& InPath) { Path = InPath; }

	virtual void UpdateShadowMap();

protected:
	virtual void Initialize_Internal() override;
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const override;
	
	std::weak_ptr<BravoShadowMap> ShadowMap;
	BravoMeshPtr Mesh;
	BravoShaderPtr Shader;

	glm::vec3 LightColor = glm::vec3(1.0f);

	std::string Path = "";
};

class BravoDirLightActor : public BravoLightActor
{
public:
	BravoDirLightActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoLightActor(_Engine, _Handle)
	{}

	virtual void Use(BravoShaderPtr OnShader) override;
	virtual void StopUsage() override;

protected:
	virtual void Initialize_Internal() override;
	virtual void OnDestroy() override;
};

class BravoPointLightActor : public BravoLightActor
{
public:
	BravoPointLightActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoLightActor(_Engine, _Handle)
	{}

	virtual void Use(BravoShaderPtr OnShader) override;
	virtual void StopUsage() override;

protected:
	virtual void Initialize_Internal() override;
	virtual void OnDestroy() override;


};

class BravoSpotLightActor : public BravoLightActor
{
public:
	BravoSpotLightActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoLightActor(_Engine, _Handle)
	{}

	virtual void Use(BravoShaderPtr OnShader) override;
	virtual void StopUsage() override;

	float CutOff;
	float OuterCutOff;
	float Constant;
	float Linear;
	float Quadratic;

protected:
	virtual void Initialize_Internal() override;
	virtual void OnDestroy() override;

};