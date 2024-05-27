#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "BravoPointDepthMap.h"
#include "BravoPointLightActor.h"

class BravoPointLightShaderDataCollection : public BravoObject
{
public:
	template <typename... Args>
	BravoPointLightShaderDataCollection(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	void Update(const std::vector<std::shared_ptr<class BravoPointLightActor>>& Casters);
	void UseOn(std::shared_ptr<class BravoShaderAsset> Shader);
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

