#include "BravoSpotLightShaderDataCollection.h"
#include "BravoSpotLightActor.h"
#include "BravoSpotDepthMap.h"
#include "BravoShaderAsset.h"
#include "BravoEngine.h"

bool BravoSpotLightShaderDataCollection::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	SpotDepthMap = NewObject<BravoSpotDepthMap>("SpotDepthMaps");
	SpotDepthMap->Setup(1, 2048);

	glGenBuffers(1, &ShaderDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	Resize(0);

	return true;
}
void BravoSpotLightShaderDataCollection::OnDestroy()
{
	glDeleteBuffers(1, &ShaderDataSSBO);
	BravoObject::OnDestroy();
}

void BravoSpotLightShaderDataCollection::Update(const std::vector<std::shared_ptr<BravoSpotLightActor>>& Casters)
{
	if ( ShaderData.size() != Casters.size() )
	{
		Resize((int32)(Casters.size()));
	}

	ShaderData.clear();
	ShaderData.reserve(Casters.size());
	for ( size_t i = 0; i < Casters.size(); ++i )
	{
		BravoSpotLightShaderData newData;
		Casters[i]->GetShaderData(newData);
		ShaderData.push_back(newData);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		BravoSpotLightShaderData* bufferData = (BravoSpotLightShaderData*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		std::memcpy(bufferData, ShaderData.data(), ShaderData.size() * sizeof(BravoSpotLightShaderData));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	SpotDepthMap->Render(ShaderData);
}
void BravoSpotLightShaderDataCollection::UseOn(std::shared_ptr<BravoShaderAsset> Shader)
{
	if ( SpotDepthMap )
	{
		SpotDepthMap->Use(Shader);
	}
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ShaderDataSSBO);

	Shader->SetInt("spotLightCount", ShaderData.size());
}

void BravoSpotLightShaderDataCollection::ResetUsage()
{
	if ( SpotDepthMap )
	{
		SpotDepthMap->StopUsage();
	}
}

void BravoSpotLightShaderDataCollection::Resize(int32 CollectionSize)
{	
	SpotDepthMap->Setup(CollectionSize, 2048);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, CollectionSize * sizeof(BravoSpotLightShaderData), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}