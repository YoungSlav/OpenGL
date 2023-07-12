#include "BravoLightActor.h"
#include "BravoEngine.h"

#include "BravoAsset.h"
#include "BravoAssetManager.h"

<<<<<<< HEAD
=======
void BravoLightActor::Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView)
{
	if ( !Mesh || !Shader )
		return;
	
	glm::mat4 model = Transform.GetTransformMatrix();
	Shader->Use();
		Shader->SetMatrix4d("projection", CameraProjection);
		Shader->SetMatrix4d("view", CameraView);
		Shader->SetMatrix4d("model", model);
		Shader->SetVector3d("lightColor", LightColor);
		// TODO
		//Mesh->Render();
	Shader->StopUsage();
	
}
>>>>>>> main


/************************************************************************/
/*                       COMMON                                         */
/************************************************************************/

void BravoDirLightActor::OnDestroy()
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
	OnShader->SetVector3d(Path + "lightColor", glm::vec3(1.0));
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
	OnShader->SetVector3d(Path + "lightColor", glm::vec3(1.0));
	OnShader->SetVector3d(Path + "position", GetLocation());
	OnShader->SetVector3d(Path + "direction", GetDirection());
	OnShader->SetVector1d(Path + "cutOff", CutOff);
	OnShader->SetVector1d(Path + "outerCutOff", OuterCutOff);
	OnShader->SetVector1d(Path + "constant", Constant);
	OnShader->SetVector1d(Path + "linear", Linear);
	OnShader->SetVector1d(Path + "quadratic", Quadratic);
}