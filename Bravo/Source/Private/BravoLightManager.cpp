#include "BravoLightManager.h"


#include "BravoEngine.h"
#include "BravoShader.h"


void BravoLightManager::RegisterLightActor(std::shared_ptr<BravoLightActor> LightActor)
{
	if ( std::shared_ptr<BravoDirLightActor> asDir = std::dynamic_pointer_cast<BravoDirLightActor>(LightActor) )
	{
		if ( !DirectionalLight.expired() )
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
void BravoLightManager::RemoveLightActor(std::shared_ptr<BravoActor> Actor)
{
	for ( unsigned int i = 0; i < SpotLights.size(); ++i )
	{
		if ( SpotLights[i].lock() == Actor )
		{
			SpotLights.erase(SpotLights.begin() + i);
			return;
		}
	}
	
	for ( unsigned int i = 0; i < PointLights.size(); ++i )
	{
		if ( PointLights[i].lock() == Actor )
		{
			PointLights.erase(PointLights.begin() + i);
			return;
		}
	}

	UpdateShaderPaths();
}

void BravoLightManager::UpdateShaderPaths()
{
	if ( !DirectionalLight.expired() )
		DirectionalLight.lock()->SetShaderPath("dirLight.");

	for ( unsigned int i = 0; i < SpotLights.size(); ++i )
	{
		if ( !SpotLights[i].expired() )
		{
			SpotLights[i].lock()->SetShaderPath("spotLights[" + std::to_string(i) + "].");
		}
	}

	for ( unsigned int i = 0; i < PointLights.size(); ++i )
	{
		if ( !PointLights[i].expired() )
		{
			PointLights[i].lock()->SetShaderPath("pointLights[" + std::to_string(i) + "].");
		}
	}
}

void BravoLightManager::UpdateLightsDepthMaps()
{
	if ( !DirectionalLight.expired() )
		DirectionalLight.lock()->UpdateShadowMap();
	for ( auto& it : SpotLights )
		it.lock()->UpdateShadowMap();
	for ( auto& it : PointLights )
		it.lock()->UpdateShadowMap();
}

void BravoLightManager::ApplyLights(std::shared_ptr<class BravoShader> Shader)
{
	if ( !DirectionalLight.expired() )
		DirectionalLight.lock()->Use(Shader);
	
	for ( auto& it : SpotLights )
		it.lock()->Use(Shader);

	Shader->SetInt("spotLightsNum", SpotLights.size());

	for ( auto& it : PointLights )
		it.lock()->Use(Shader);
	
	Shader->SetInt("pointLightsNum", PointLights.size());
}
void BravoLightManager::ResetLightsUsage()
{
	if ( !DirectionalLight.expired() )
		DirectionalLight.lock()->StopUsage();
	for ( auto& it : SpotLights )
		it.lock()->StopUsage();
	for ( auto& it : PointLights )
		it.lock()->StopUsage();
}