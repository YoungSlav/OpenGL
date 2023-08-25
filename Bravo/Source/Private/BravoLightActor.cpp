#include "BravoLightActor.h"
#include "BravoEngine.h"

#include "BravoAsset.h"
#include "BravoAssetManager.h"


/************************************************************************/
/*                       COMMON                                         */
/************************************************************************/

BravoLightAttenuationConstants BravoLightActor::LightAttenuationConstants;

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
