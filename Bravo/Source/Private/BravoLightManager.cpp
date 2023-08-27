#include "BravoLightManager.h"
#include "BravoDirectionalLightActor.h"
#include "BravoPointLightActor.h"

#include "BravoSpotLightActor.h"
#include "BravoSpotLightShaderDataCollection.h"
#include "BravoDirectionalLightShaderDataCollection.h"

#include "BravoEngine.h"
#include "BravoShader.h"

bool BravoLightManager::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	SpotLightsDataCollection = NewObject<BravoSpotLightShaderDataCollection>("SpotLightShaderDataCollection");
	DirectionalLightsDataCollection = NewObject<BravoDirectionalLightShaderDataCollection>("DirectionalLightsDataCollection");

	return true;
}

void BravoLightManager::RegisterLightActor(std::shared_ptr<BravoLightActor> LightActor)
{
	if ( std::shared_ptr<BravoDirectionalLightActor> asDir = std::dynamic_pointer_cast<BravoDirectionalLightActor>(LightActor) )
	{
		DirectionalLights.push_back(asDir);
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
	DirectionalLights.erase(std::remove(DirectionalLights.begin(), DirectionalLights.end(), lightActor), DirectionalLights.end());
}

void BravoLightManager::UpdateLightsShaderData()
{	
	DirectionalLightsDataCollection->Update(DirectionalLights);
	SpotLightsDataCollection->Update(SpotLights);

	for ( auto& it : PointLights )
		it->UpdateDepthMap();
}

void BravoLightManager::ApplyLights(std::shared_ptr<class BravoShader> Shader)
{
	DirectionalLightsDataCollection->UseOn(Shader);
	
	SpotLightsDataCollection->UseOn(Shader);
	
	Shader->SetInt("pointLightsNum", PointLights.size());
	for ( auto& it : PointLights )
		it->Apply(Shader);
}

void BravoLightManager::ResetLightsUsage()
{
	DirectionalLightsDataCollection->ResetUsage();

	SpotLightsDataCollection->ResetUsage();
	
	for ( auto& it : PointLights )
		it->StopUsage();
}