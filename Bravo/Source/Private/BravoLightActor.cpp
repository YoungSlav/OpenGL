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



/************************************************************************/
/*                       DIRECTIONAL LIGHT                              */
/************************************************************************/

bool BravoDirLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	ShadowMap = NewObject<BravoShadowMap_Directional>();
	ShadowMap->Setup(glm::ivec2(2048));

	return true;
}

void BravoDirLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	OnShader->SetVector3d(Path + "direction", GetDirection());
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
	OnShader->SetVector3d(Path + "light.diffuse", LightColor.diffuse);
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
}



/************************************************************************/
/*                           POINT LIGHT                                */
/************************************************************************/

bool BravoPointLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	ShadowMap = NewObject<BravoShadowMap_Point>();
	ShadowMap->Setup(glm::ivec2(2048));
	
	return true;
}

void BravoPointLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	// TODO
}




/************************************************************************/
/*                           SPOT LIGHT                                 */
/************************************************************************/

bool BravoSpotLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	ShadowMap = NewObject<BravoShadowMap_Spot>();
	ShadowMap->Setup(glm::ivec2(2048));

	return true;
}


void BravoSpotLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
	OnShader->SetVector3d(Path + "light.diffuse", LightColor.diffuse);
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);

	OnShader->SetVector3d(Path + "position", GetLocation());
	OnShader->SetVector3d(Path + "direction", GetDirection());
	OnShader->SetVector1d(Path + "cutOff", CutOff);
	OnShader->SetVector1d(Path + "outerCutOff", OuterCutOff);
	OnShader->SetVector1d(Path + "constant", Constant);
	OnShader->SetVector1d(Path + "linear", Linear);
	OnShader->SetVector1d(Path + "quadratic", Quadratic);
}