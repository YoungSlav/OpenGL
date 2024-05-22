#include "BravoPointLightShaderDataCollection.h"
#include "BravoPointLightActor.h"
#include "BravoPointDepthMap.h"
#include "BravoShaderAsset.h"
#include "BravoEngine.h"

bool BravoPointLightShaderDataCollection::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	PointDepthMap = NewObject<BravoPointDepthMap>("PointDepthMaps");
	PointDepthMap->Setup(1, 2048);

	glGenBuffers(1, &ShaderDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	Resize(0);

	return true;
}

void BravoPointLightShaderDataCollection::OnDestroy()
{
	glDeleteBuffers(1, &ShaderDataSSBO);
	BravoObject::OnDestroy();
}

void BravoPointLightShaderDataCollection::Update(const std::vector<std::shared_ptr<BravoPointLightActor>>& Casters)
{
	if ( ShaderData.size() != Casters.size() )
	{
		Resize((int32)(Casters.size()));
	}

	ShaderData.clear();
	ShaderData.reserve(Casters.size());
	for ( size_t i = 0; i < Casters.size(); ++i )
	{
		BravoPointLightShaderData newData;
		Casters[i]->GetShaderData(newData);
		ShaderData.push_back(newData);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		BravoPointLightShaderData* bufferData = (BravoPointLightShaderData*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		std::memcpy(bufferData, ShaderData.data(), ShaderData.size() * sizeof(BravoPointLightShaderData));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	PointDepthMap->Render(ShaderData);
}

void BravoPointLightShaderDataCollection::UseOn(std::shared_ptr<BravoShaderAsset> Shader)
{
	if ( PointDepthMap )
	{
		PointDepthMap->Use(Shader);
	}
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ShaderDataSSBO);

	Shader->SetInt("pointLightCount", ShaderData.size());
}

void BravoPointLightShaderDataCollection::ResetUsage()
{
	if ( PointDepthMap )
	{
		PointDepthMap->StopUsage();
	}
}

void BravoPointLightShaderDataCollection::Resize(int32 CollectionSize)
{	
	PointDepthMap->Setup(CollectionSize*6, 2048);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, CollectionSize * sizeof(BravoPointLightShaderData), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}