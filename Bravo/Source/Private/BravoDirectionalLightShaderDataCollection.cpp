#include "BravoDirectionalLightShaderDataCollection.h"
#include "BravoEngine.h"

bool BravoDirectionalLightShaderDataCollection::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	DirectionalDepthMap = NewObject<BravoDirectionalDepthMap>("DirectionalDepthMaps");
	DirectionalDepthMap->Setup(1, 4096);

	glGenBuffers(1, &ShaderDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return true;
}

void BravoDirectionalLightShaderDataCollection::OnDestroy()
{
	glDeleteBuffers(1, &ShaderDataSSBO);
	BravoObject::OnDestroy();
}

void BravoDirectionalLightShaderDataCollection::Update(const std::vector<std::shared_ptr<BravoDirectionalLightActor>>& Casters)
{
	int32 CollectionSize = 0;
	for ( auto it : Casters )
		CollectionSize += (int32)it->GetSettings().ShadowCascadeLevels.size();

	if ( ShaderData.size() != CollectionSize )
	{
		Resize(CollectionSize);
	}

	ShaderData.clear();
	ShaderData.reserve(CollectionSize);
	for ( size_t i = 0; i < Casters.size(); ++i )
	{
		Casters[i]->GetShaderData(ShaderData);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		BravoDirectionalLightShaderData* bufferData = (BravoDirectionalLightShaderData*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		std::memcpy(bufferData, ShaderData.data(), ShaderData.size() * sizeof(BravoDirectionalLightShaderData));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	DirectionalDepthMap->Render(ShaderData);

}

void BravoDirectionalLightShaderDataCollection::UseOn(std::shared_ptr<BravoShaderAsset> Shader)
{
	if ( DirectionalDepthMap )
	{
		DirectionalDepthMap->Use(Shader);
	}
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ShaderDataSSBO);

	Shader->SetInt("directionalLightCount", ShaderData.size());
}

void BravoDirectionalLightShaderDataCollection::ResetUsage()
{
	if ( DirectionalDepthMap )
	{
		DirectionalDepthMap->StopUsage();
	}
}

void BravoDirectionalLightShaderDataCollection::Resize(int32 CollectionSize)
{
	DirectionalDepthMap->Setup(CollectionSize, 4096);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, CollectionSize * sizeof(BravoDirectionalLightShaderData), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}