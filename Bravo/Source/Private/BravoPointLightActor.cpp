#include "stdafx.h"

#include "BravoPointLightActor.h"
#include "BravoShaderAsset.h"
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
}

void BravoPointLightActor::GetShaderData(BravoPointLightShaderData& OutShaderData) const
{
	OutShaderData.Color = GetLightColor() * Settings.Intencity;

	OutShaderData.Position = GetLocation();

	OutShaderData.FarPlane = Settings.Intencity;
}