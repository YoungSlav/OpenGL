#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "BravoSpotDepthMap.h"
#include "BravoSpotLightActor.h"

class BravoSpotLightShaderDataCollection : public BravoObject
{
public:
	template <typename... Args>
	BravoSpotLightShaderDataCollection(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	void Update(const std::vector<std::shared_ptr<class BravoSpotLightActor>>& Casters);
	void UseOn(std::shared_ptr<class BravoShaderAsset> Shader);
	void ResetUsage();

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	void Resize(int32 CollectionSize);

	

private:
	std::shared_ptr<class BravoSpotDepthMap> SpotDepthMap = nullptr;


	uint32 ShaderDataSSBO = 0;
	std::vector<BravoSpotLightShaderData> ShaderData;
};
