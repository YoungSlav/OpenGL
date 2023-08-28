#include "stdafx.h"

#include "BravoPointLightActor.h"
#include "BravoShader.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"

bool BravoPointLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	return true;
}

void BravoPointLightActor::SetSettings(BravoPointLightSettings _Settings)
{
	Settings = _Settings;
	Settings.Intencity = std::min(Settings.Intencity, (uint32)(LightAttenuationConstants.Distance.size()-1));
}

void BravoPointLightActor::GetShaderData(BravoPointLightShaderData& OutShaderData) const
{
	OutShaderData.AmbientLight = LightColor.ambient;
	OutShaderData.DiffuseLight = LightColor.diffuse;
	OutShaderData.SpecularLight = LightColor.specular;

	OutShaderData.Position = GetLocation();

	OutShaderData.Constant = LightAttenuationConstants.Constant[Settings.Intencity];
	OutShaderData.Linear = LightAttenuationConstants.Linear[Settings.Intencity];
	OutShaderData.Quadric = LightAttenuationConstants.Quadratic[Settings.Intencity];
	OutShaderData.FarPlane = LightAttenuationConstants.Distance[Settings.Intencity];
}