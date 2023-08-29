#include "BravoSpotLightActor.h"

#include "BravoShaderAsset.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"

bool BravoSpotLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	return true;
}

void BravoSpotLightActor::SetSettings(BravoSpotLightSettings _Settings)
{
	Settings = _Settings;
}

void BravoSpotLightActor::GetLightSpaceTransformationMatrix(glm::mat4& OutTransformationMatrix) const
{
	const float NearPlane = 0.1f;	
	const float FarPlane = Settings.MaxDistance;

	const float FOV = glm::radians(Settings.OuterCutOff * 2.0f);
	const float AspectRatio = 1.0f;
	
	const glm::mat4 ShadowProjection = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);
	const glm::mat4 LightView = glm::lookAt(GetLocation(), GetLocation() + GetDirection(), glm::vec3(0.0f, 1.0, 0.0f));

	OutTransformationMatrix = ShadowProjection * LightView;
}

void BravoSpotLightActor::GetShaderData(BravoSpotLightShaderData& OutShaderData) const
{
	OutShaderData.Color = GetLightColor();

	OutShaderData.Position = GetLocation();
	OutShaderData.Direction = GetDirection();

	OutShaderData.CutOff = glm::cos(glm::radians(Settings.CutOff));
	OutShaderData.OuterCutOff = glm::cos(glm::radians(Settings.OuterCutOff));

	OutShaderData.FarPlane = Settings.MaxDistance;

	GetLightSpaceTransformationMatrix(OutShaderData.LightSpaceMatrix);
}