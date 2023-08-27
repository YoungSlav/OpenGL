#include "BravoDirectionalLightShaderDataCollection.h"
#include "BravoEngine.h"

bool BravoDirectionalLightShaderDataCollection::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	glGenBuffers(1, &ShaderDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	Resize(0);
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
	for ( int32 i = 0; i < Casters.size(); ++i )
	{
		int32 StartingLayer = (int32)ShaderData.size();
		Casters[i]->GetShaderData(ShaderData);
		for ( int32 layer = StartingLayer; layer < ShaderData.size(); ++layer)
			DirectionalDepthMap->Render(layer, ShaderData[layer]);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		BravoDirectionalLightShaderData* bufferData = (BravoDirectionalLightShaderData*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		std::memcpy(bufferData, ShaderData.data(), ShaderData.size() * sizeof(BravoDirectionalLightShaderData));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void BravoDirectionalLightShaderDataCollection::UseOn(std::shared_ptr<BravoShader> Shader)
{
	if ( DirectionalDepthMap )
	{
		DirectionalDepthMap->Use(Shader);
	}
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ShaderDataSSBO);
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
	if ( DirectionalDepthMap )
		DirectionalDepthMap->Destroy();

	DirectionalDepthMap = NewObject<BravoDirectionalDepthMap>("DirectionalDepthMaps");
	DirectionalDepthMap->Setup(CollectionSize);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, CollectionSize * sizeof(BravoDirectionalLightShaderData), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}