#include "BravoLightManager.h"

#include "BravoDirectionalLightActor.h"
#include "BravoPointLightActor.h"
#include "BravoSpotLightActor.h"

#include "BravoDirectionalLightShaderDataCollection.h"
#include "BravoSpotLightShaderDataCollection.h"
#include "BravoPointLightShaderDataCollection.h"

#include "BravoEngine.h"
#include "BravoShaderAsset.h"

bool BravoLightManager::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	SpotLightsDataCollection = NewObject<BravoSpotLightShaderDataCollection>("SpotLightShaderDataCollection");
	DirectionalLightsDataCollection = NewObject<BravoDirectionalLightShaderDataCollection>("DirectionalLightsDataCollection");
	PointLightsDataCollection = NewObject<BravoPointLightShaderDataCollection>("PointLightShaderDataCollection");

	return true;
}

void BravoLightManager::RegisterLightActor(std::shared_ptr<BravoLightActor> LightActor)
{
	if ( std::shared_ptr<BravoDirectionalLightActor> asDir = std::dynamic_pointer_cast<BravoDirectionalLightActor>(LightActor) )
		DirectionalLights.push_back(asDir);

	if ( std::shared_ptr<BravoSpotLightActor> asSpot = std::dynamic_pointer_cast<BravoSpotLightActor>(LightActor) )
		SpotLights.push_back(asSpot);

	if ( std::shared_ptr<BravoPointLightActor> asPoint = std::dynamic_pointer_cast<BravoPointLightActor>(LightActor) )
		PointLights.push_back(asPoint);
}

void BravoLightManager::RemoveLightActor(std::shared_ptr<BravoLightActor> lightActor)
{
	SpotLights.erase(std::remove(SpotLights.begin(), SpotLights.end(), lightActor), SpotLights.end());
	PointLights.erase(std::remove(PointLights.begin(), PointLights.end(), lightActor), PointLights.end());
	DirectionalLights.erase(std::remove(DirectionalLights.begin(), DirectionalLights.end(), lightActor), DirectionalLights.end());
}

void BravoLightManager::UpdateLightsShaderData()
{	
	if ( DirectionalLights.size() )
		DirectionalLightsDataCollection->Update(DirectionalLights);
	if ( SpotLights.size() )
		SpotLightsDataCollection->Update(SpotLights);
	if ( PointLights.size() )
		PointLightsDataCollection->Update(PointLights);
}

void BravoLightManager::ApplyLights(std::shared_ptr<class BravoShaderAsset> Shader)
{
	if ( DirectionalLights.size() )
		DirectionalLightsDataCollection->UseOn(Shader);
	
	if ( SpotLights.size() )
		SpotLightsDataCollection->UseOn(Shader);

	if ( PointLights.size() )
		PointLightsDataCollection->UseOn(Shader);
}

void BravoLightManager::ResetLightsUsage()
{
	if ( DirectionalLights.size() )
		DirectionalLightsDataCollection->ResetUsage();

	if ( SpotLights.size() )
		SpotLightsDataCollection->ResetUsage();
	
	if ( PointLights.size() )
		PointLightsDataCollection->ResetUsage();
}