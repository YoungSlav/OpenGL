#include "BravoLightManager.h"
#include "BravoDirectionalLightActor.h"
#include "BravoSpotLightActor.h"
#include "BravoPointLightActor.h"
#include "BravoSpotDepthMap.h"

#include "BravoEngine.h"
#include "BravoShader.h"


void BravoLightManager::RegisterLightActor(std::shared_ptr<BravoLightActor> LightActor)
{
	if ( std::shared_ptr<BravoDirectionalLightActor> asDir = std::dynamic_pointer_cast<BravoDirectionalLightActor>(LightActor) )
	{
		if ( DirectionalLight )
		{
			Log::LogMessage("Directional light override!", ELog::Warning);
		}
		DirectionalLight = asDir;
	}

	if ( std::shared_ptr<BravoSpotLightActor> asSpot = std::dynamic_pointer_cast<BravoSpotLightActor>(LightActor) )
	{
		SpotLights.push_back(asSpot);
		SpawnSpotDepthMap();
	}

	if ( std::shared_ptr<BravoPointLightActor> asPoint = std::dynamic_pointer_cast<BravoPointLightActor>(LightActor) )
	{
		PointLights.push_back(asPoint);
	}

	UpdateShaderPaths();
}
void BravoLightManager::RemoveLightActor(std::shared_ptr<BravoLightActor> lightActor)
{
	SpotLights.erase(std::remove(SpotLights.begin(), SpotLights.end(), lightActor), SpotLights.end());
	PointLights.erase(std::remove(PointLights.begin(), PointLights.end(), lightActor), PointLights.end());
	
	if ( DirectionalLight == lightActor )
		DirectionalLight.reset();


	if ( std::shared_ptr<BravoDirectionalLightActor> asDir = std::dynamic_pointer_cast<BravoDirectionalLightActor>(lightActor) )
	{
	}

	if ( std::shared_ptr<BravoSpotLightActor> asSpot = std::dynamic_pointer_cast<BravoSpotLightActor>(lightActor) )
	{
		SpawnSpotDepthMap();
	}

	if ( std::shared_ptr<BravoPointLightActor> asPoint = std::dynamic_pointer_cast<BravoPointLightActor>(lightActor) )
	{
	}

	UpdateShaderPaths();
}

void BravoLightManager::SpawnSpotDepthMap()
{
	if ( SpotDepthMap )
		SpotDepthMap->Destroy();

	SpotDepthMap = NewObject<BravoSpotDepthMap>("SpotDepthMaps");
}

void BravoLightManager::UpdateShaderPaths()
{
	if ( DirectionalLight )
		DirectionalLight->SetShaderPath("dirLight.");

	for ( int32 i = 0; i < (int32)SpotLights.size(); ++i )
	{
		if ( SpotLights[i] )
		{
			SpotLights[i]->SetShaderPath("spotLights[" + std::to_string(i) + "].");
			SpotLights[i]->SetDepthMapLayer(i);
		}
	}

	for ( int32 i = 0; i < (int32)PointLights.size(); ++i )
	{
		if ( PointLights[i] )
		{
			PointLights[i]->SetShaderPath("pointLights[" + std::to_string(i) + "].");
		}
	}
}

void BravoLightManager::UpdateLightsDepthMaps()
{
	if ( DirectionalLight )
		DirectionalLight->UpdateDepthMap();
	
	if ( SpotDepthMap )
	{
		for ( auto& it : SpotLights )
		{
			SpotDepthMap->Render(it);
		}
	}

	for ( auto& it : PointLights )
		it->UpdateDepthMap();
}

void BravoLightManager::ApplyLights(std::shared_ptr<class BravoShader> Shader)
{
	if ( DirectionalLight )
		DirectionalLight->Apply(Shader);
	
	Shader->SetInt("spotLightsNum", SpotLights.size());
	for ( auto& it : SpotLights )
		it->Apply(Shader);
	
	if ( SpotDepthMap )
		SpotDepthMap->Use(Shader);


	
	Shader->SetInt("pointLightsNum", PointLights.size());
	for ( auto& it : PointLights )
		it->Apply(Shader);
}

void BravoLightManager::ResetLightsUsage()
{
	if ( SpotDepthMap )
		SpotDepthMap->StopUsage();
	if ( DirectionalLight )
		DirectionalLight->StopUsage();
	for ( auto& it : SpotLights )
		it->StopUsage();
	for ( auto& it : PointLights )
		it->StopUsage();
}