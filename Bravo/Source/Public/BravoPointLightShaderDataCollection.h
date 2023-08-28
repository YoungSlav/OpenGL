#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "BravoPointDepthMap.h"
#include "BravoPointLightActor.h"

class BravoPointLightShaderDataCollection : public BravoObject
{
public:
	void Update(const std::vector<std::shared_ptr<class BravoPointLightActor>>& Casters);
	void UseOn(std::shared_ptr<class BravoShader> Shader);
	void ResetUsage();

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	void Resize(int32 CollectionSize);

	

private:
	std::shared_ptr<class BravoPointDepthMap> PointDepthMap = nullptr;


	uint32 ShaderDataSSBO = 0;
	std::vector<BravoPointLightShaderData> ShaderData;
};
