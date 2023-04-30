#include "BravoLightManager.h"

#include "BravoViewport.h"
#include "BravoEngine.h"
#include "BravoShader.h"


void BravoLightManager::RegisterLightActor(BravoLightActor* LightActor)
{
	if ( BravoDirLightActor* asDir = dynamic_cast<BravoDirLightActor*>(LightActor) )
	{
		if ( DirectionalLight != nullptr )
		{
			Log::LogMessage("Directional light override!", ELog::Warning);
		}
		DirectionalLight = asDir;
	}

	if ( BravoSpotLightActor* asSpot = dynamic_cast<BravoSpotLightActor*>(LightActor) )
	{
		SpotLights.push_back(asSpot);
	}

	if ( BravoPointLightActor* asSpot = dynamic_cast<BravoPointLightActor*>(LightActor) )
	{
		PointLights.push_back(asSpot);
	}

	UpdateShaderPaths();
}
void BravoLightManager::RemoveLightActor(BravoObjectHandle InHandle)
{
	if ( DirectionalLight->GetHandle() == InHandle )
	{
		DirectionalLight = nullptr;
		return;
	}

	for ( unsigned int i = 0; i < SpotLights.size(); ++i )
	{
		if ( SpotLights[i]->GetHandle() == InHandle )
		{
			SpotLights.erase(SpotLights.begin() + i);
			return;
		}
	}

	for ( unsigned int i = 0; i < PointLights.size(); ++i )
	{
		if ( PointLights[i]->GetHandle() == InHandle )
		{
			PointLights.erase(PointLights.begin() + i);
			return;
		}
	}

	UpdateShaderPaths();
}

void BravoLightManager::UpdateShaderPaths()
{
	if ( DirectionalLight != nullptr )
		DirectionalLight->SetShaderPath("dirLight.");

	for ( unsigned int i = 0; i < SpotLights.size(); ++i )
		SpotLights[i]->SetShaderPath("spotLights[" + std::to_string(i) + "].");

	for ( unsigned int i = 0; i < PointLights.size(); ++i )
		PointLights[i]->SetShaderPath("pointLights[" + std::to_string(i) + "].");
}

void BravoLightManager::UpdateLightsDepthMaps()
{
	DirectionalLight->UpdateShadowMap();
	for ( auto& it : SpotLights )
		it->UpdateShadowMap();
	for ( auto& it : PointLights )
		it->UpdateShadowMap();
}

void BravoLightManager::ApplyLights(std::shared_ptr<class BravoShader> Shader)
{
	DirectionalLight->Use(Shader);
	
	for ( auto& it : SpotLights )
		it->Use(Shader);

	Shader->SetInt("spotLightsNum", SpotLights.size());

	for ( auto& it : PointLights )
		it->Use(Shader);
	
	Shader->SetInt("pointLightsNum", PointLights.size());
}
void BravoLightManager::ResetLightsUsage()
{
	DirectionalLight->StopUsage();
	for ( auto& it : SpotLights )
		it->StopUsage();
	for ( auto& it : PointLights )
		it->StopUsage();
}