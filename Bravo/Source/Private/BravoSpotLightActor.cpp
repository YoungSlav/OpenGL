#include "BravoSpotLightActor.h"

#include "BravoShaderAsset.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"

RTTR_REGISTRATION
{
	using namespace rttr;

	registration::class_<BravoSpotLightSettings>("SpotLightSettings")
		.property("CutOff", &BravoSpotLightSettings::CutOff)
		.property("OuterCutOff", &BravoSpotLightSettings::OuterCutOff)
		.property("Intencity", &BravoSpotLightSettings::Intencity)
		;

	registration::class_<BravoSpotLightActor>("SpotLightActor")
		.property("Settings", &BravoSpotLightActor::GetSettings, &BravoSpotLightActor::SetSettings)
		;
}

bool BravoSpotLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	return true;
}

void BravoSpotLightActor::SetSettings(const BravoSpotLightSettings& _Settings)
{
	Settings = _Settings;
}

void BravoSpotLightActor::GetLightSpaceTransformationMatrix(glm::mat4& OutTransformationMatrix) const
{
	const float NearPlane = 0.1f;	
	const float FarPlane = Settings.Intencity;

	const float FOV = glm::radians(Settings.OuterCutOff * 2.0f);
	const float AspectRatio = 1.0f;
	
	const glm::mat4 ShadowProjection = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);
	const glm::mat4 LightView = glm::lookAt(GetLocation(), GetLocation() + GetForwardVector_World(), glm::vec3(0.0f, 1.0, 0.0f));

	OutTransformationMatrix = ShadowProjection * LightView;
}

void BravoSpotLightActor::GetShaderData(BravoSpotLightShaderData& OutShaderData) const
{
	OutShaderData.Color = GetLightColor() * Settings.Intencity;

	OutShaderData.Position = GetLocation();
	OutShaderData.Direction = GetForwardVector_World();

	OutShaderData.CutOff = glm::cos(glm::radians(Settings.CutOff));
	OutShaderData.OuterCutOff = glm::cos(glm::radians(Settings.OuterCutOff));

	OutShaderData.FarPlane = Settings.Intencity;

	GetLightSpaceTransformationMatrix(OutShaderData.LightSpaceMatrix);
}