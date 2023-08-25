#include "BravoLightActor.h"
#include "BravoEngine.h"

#include "BravoAsset.h"
#include "BravoAssetManager.h"


/************************************************************************/
/*                       COMMON                                         */
/************************************************************************/

void BravoLightActor::OnDestroy()
{
	DepthMap.reset();
	BravoActor::OnDestroy();
}

void BravoLightActor::UpdateDepthMap()
{
	DepthMap->Render(Self<BravoLightActor>());
}

void BravoLightActor::Use(BravoShaderPtr OnShader)
{
	DepthMap->Use(OnShader, Path);
}
void BravoLightActor::StopUsage()
{
	DepthMap->StopUsage();
}


bool BravoPointLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	DepthMap = NewObject<BravoDepthMap_Point>();
	DepthMap->Setup(2048);
	
	return true;
}

void BravoPointLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	// TODO
}

