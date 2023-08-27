#include "BravoSpotLightActor.h"

#include "BravoShader.h"
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
	Settings.Intencity = std::min(Settings.Intencity, (uint32)(LightAttenuationConstants.Distance.size()-1));
}

void BravoSpotLightActor::GetLightSpaceTransformationMatrix(glm::mat4& OutTransformationMatrix) const
{
	const float NearPlane = 0.1f;	
	const float FarPlane = LightAttenuationConstants.Distance[Settings.Intencity];

	const float FOV = glm::radians(Settings.OuterCutOff * 2.0f);
	const float AspectRatio = 1.0f;
	
	const glm::mat4 ShadowProjection = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);
	const glm::mat4 LightView = glm::lookAt(GetLocation(), GetLocation() + GetDirection(), glm::vec3(0.0f, 1.0, 0.0f));

	OutTransformationMatrix = ShadowProjection * LightView;
}

void BravoSpotLightActor::GetShaderData(BravoSpotLightShaderData& OutShaderData) const
{
	OutShaderData.AmbientLight = LightColor.ambient;
	OutShaderData.DiffuseLight = LightColor.diffuse;
	OutShaderData.SpecularLight = LightColor.specular;

	OutShaderData.Position = GetLocation();
	OutShaderData.Direction = GetDirection();

	OutShaderData.CutOff = glm::cos(glm::radians(Settings.CutOff));
	OutShaderData.OuterCutOff = glm::cos(glm::radians(Settings.OuterCutOff));

	OutShaderData.Constant = LightAttenuationConstants.Constant[Settings.Intencity];
	OutShaderData.Linear = LightAttenuationConstants.Linear[Settings.Intencity];
	OutShaderData.Quadric = LightAttenuationConstants.Quadratic[Settings.Intencity];
	OutShaderData.FarPlane = LightAttenuationConstants.Distance[Settings.Intencity];
	GetLightSpaceTransformationMatrix(OutShaderData.LightSpaceMatrix);
}