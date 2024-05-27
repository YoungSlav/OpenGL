#pragma once


#include "stdafx.h"
#include "BravoObject.h"
#include "BravoDirectionalDepthMap.h"
#include "BravoDirectionalLightActor.h"

class BravoDirectionalLightShaderDataCollection : public BravoObject
{
public:
	template <typename... Args>
	BravoDirectionalLightShaderDataCollection(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	void Update(const std::vector<std::shared_ptr<class BravoDirectionalLightActor>>& Casters);
	void UseOn(std::shared_ptr<class BravoShaderAsset> Shader);
	void ResetUsage();

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	void Resize(int32 CollectionSize);

private:
	std::shared_ptr<class BravoDirectionalDepthMap> DirectionalDepthMap = nullptr;

	
	uint32 ShaderDataSSBO = 0;
	std::vector<BravoDirectionalLightShaderData> ShaderData;
};