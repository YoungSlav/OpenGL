#include "BravoLightManager.h"
#include "BravoDirectionalLightActor.h"
#include "BravoSpotLightActor.h"
#include "BravoPointLightActor.h"

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
	}

	if ( std::shared_ptr<BravoPointLightActor> asSpot = std::dynamic_pointer_cast<BravoPointLightActor>(LightActor) )
	{
		PointLights.push_back(asSpot);
	}

	UpdateShaderPaths();
}
void BravoLightManager::RemoveLightActor(std::shared_ptr<BravoLightActor> lightActor)
{
	SpotLights.erase(std::remove(SpotLights.begin(), SpotLights.end(), lightActor), SpotLights.end());
	PointLights.erase(std::remove(PointLights.begin(), PointLights.end(), lightActor), PointLights.end());
	
	if ( DirectionalLight == lightActor )
		DirectionalLight.reset();

	UpdateShaderPaths();
}

void BravoLightManager::UpdateShaderPaths()
{
	if ( DirectionalLight )
		DirectionalLight->SetShaderPath("dirLight.");

	for ( uint32 i = 0; i < SpotLights.size(); ++i )
	{
		if ( SpotLights[i] )
		{
			SpotLights[i]->SetShaderPath("spotLights[" + std::to_string(i) + "].");
		}
	}

	for ( uint32 i = 0; i < PointLights.size(); ++i )
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
	for ( auto& it : SpotLights )
		it->UpdateDepthMap();
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


	
	Shader->SetInt("pointLightsNum", PointLights.size());
	for ( auto& it : PointLights )
		it->Apply(Shader);
}

void BravoLightManager::ResetLightsUsage()
{
	if ( DirectionalLight )
		DirectionalLight->StopUsage();
	for ( auto& it : SpotLights )
		it->StopUsage();
	for ( auto& it : PointLights )
		it->StopUsage();
}