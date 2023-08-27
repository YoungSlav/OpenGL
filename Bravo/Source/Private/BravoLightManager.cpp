#include "BravoLightManager.h"
#include "BravoDirectionalLightActor.h"
#include "BravoPointLightActor.h"

#include "BravoSpotLightActor.h"
#include "BravoSpotLightShaderDataCollection.h"

#include "BravoEngine.h"
#include "BravoShader.h"

bool BravoLightManager::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	SpotLightsDataCollection = NewObject<BravoSpotLightShaderDataCollection>("SpotLightShaderDataCollection");

	return true;
}

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

	if ( std::shared_ptr<BravoPointLightActor> asPoint = std::dynamic_pointer_cast<BravoPointLightActor>(LightActor) )
	{
		PointLights.push_back(asPoint);
	}
}
void BravoLightManager::RemoveLightActor(std::shared_ptr<BravoLightActor> lightActor)
{
	SpotLights.erase(std::remove(SpotLights.begin(), SpotLights.end(), lightActor), SpotLights.end());
	PointLights.erase(std::remove(PointLights.begin(), PointLights.end(), lightActor), PointLights.end());
	
	if ( DirectionalLight == lightActor )
		DirectionalLight.reset();
}

void BravoLightManager::UpdateLightsShaderData()
{
	if ( DirectionalLight )
		DirectionalLight->UpdateDepthMap();
	

	SpotLightsDataCollection->Update(SpotLights);

	for ( auto& it : PointLights )
		it->UpdateDepthMap();
}

void BravoLightManager::ApplyLights(std::shared_ptr<class BravoShader> Shader)
{
	if ( DirectionalLight )
		DirectionalLight->Apply(Shader);
	
	SpotLightsDataCollection->UseOn(Shader);
	
	Shader->SetInt("pointLightsNum", PointLights.size());
	for ( auto& it : PointLights )
		it->Apply(Shader);
}

void BravoLightManager::ResetLightsUsage()
{
	if ( DirectionalLight )
		DirectionalLight->StopUsage();

	SpotLightsDataCollection->ResetUsage();
	
	for ( auto& it : PointLights )
		it->StopUsage();
}