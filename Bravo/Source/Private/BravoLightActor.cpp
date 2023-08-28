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
	//DepthMap->Render(Self<BravoLightActor>());
}

void BravoLightActor::Apply(std::shared_ptr<BravoShaderAsset> OnShader)
{
	//DepthMap->Apply(OnShader, Path);
}
void BravoLightActor::StopUsage()
{
	//DepthMap->StopUsage();
}
