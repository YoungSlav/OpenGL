#include "BravoLightActor.h"
#include "BravoEngine.h"

#include "BravoAsset.h"
#include "BravoAssetManager.h"


/************************************************************************/
/*                       COMMON                                         */
/************************************************************************/

void BravoLightActor::OnDestroy()
{
	ShadowMap.reset();
	BravoActor::OnDestroy();
}

void BravoLightActor::UpdateShadowMap()
{
	ShadowMap->Render(Self<BravoLightActor>());
}

void BravoLightActor::Use(BravoShaderPtr OnShader)
{
	ShadowMap->Use(OnShader, Path);
}
void BravoLightActor::StopUsage()
{
	ShadowMap->StopUsage();
}

/*
/************************************************************************/
/*                           POINT LIGHT                                */
/************************************************************************/

bool BravoPointLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	ShadowMap = NewObject<BravoShadowMap_Point>();
	ShadowMap->Setup(2048);
	
	return true;
}

void BravoPointLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	// TODO
}

