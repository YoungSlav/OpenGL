#include "BravoLightActor.h"
#include "BravoEngine.h"

#include "BravoAsset.h"
#include "BravoAssetManager.h"

void BravoLightActor::Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	if ( Mesh && Shader )
	{
		glm::mat4 model = GetTransformMatrix();
		Shader->Use();
			Shader->SetMatrix4d("projection", CameraProjection);
			Shader->SetMatrix4d("view", CameraView);
			Shader->SetMatrix4d("model", model);
			Shader->SetVector3d("lightColor", LightColor);
			Mesh->Render();
		Shader->StopUsage();
	}
}

bool BravoLightActor::Initialize_Internal()
{
	Shader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\LightSource");
	if ( Shader->LoadToGPU() )
		return true;
	return false;
}

void BravoLightActor::UpdateShadowMap()
{
	ShadowMap.lock()->Render(Self<BravoLightActor>());
}

void BravoLightActor::Use(BravoShaderPtr OnShader)
{
	ShadowMap.lock()->Use(OnShader, Path);
}
void BravoLightActor::StopUsage()
{
	ShadowMap.lock()->StopUsage();
}


bool BravoDirLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	Mesh = Engine->GetAssetManager()->LoadAsset<BravoMesh>("primitives\\cone.fbx");
	if ( !Mesh || !Mesh->LoadToGPU() )
		return false;

	ShadowMap = Engine->SpawnObject<BravoShadowMap_Directional>();
	ShadowMap.lock()->Setup(glm::ivec2(2048));

	return true;
}
void BravoDirLightActor::OnDestroy()
{
	Mesh->ReleaseFromGPU();
	Shader->ReleaseFromGPU();
}

void BravoDirLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	OnShader->SetVector3d(Path + "direction", GetDirection());
	OnShader->SetVector3d(Path + "lightColor", glm::vec3(1.0));
}
void BravoDirLightActor::StopUsage()
{
	BravoLightActor::StopUsage();
}


bool BravoPointLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	Mesh = Engine->GetAssetManager()->LoadAsset<BravoMesh>("primitives\\sphere.fbx");
	if ( !Mesh || !Mesh->LoadToGPU() )
		return false;

	ShadowMap = Engine->SpawnObject<BravoShadowMap_Point>();
	ShadowMap.lock()->Setup(glm::ivec2(2048));
	
	return true;
}

void BravoPointLightActor::OnDestroy()
{
	Mesh->ReleaseFromGPU();
	Shader->ReleaseFromGPU();
}

void BravoPointLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	// TODO
}
void BravoPointLightActor::StopUsage()
{
	BravoLightActor::StopUsage();
	// TODO
}






bool BravoSpotLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	Mesh = Engine->GetAssetManager()->LoadAsset<BravoMesh>("primitives\\cone.fbx");
	if ( !Mesh || !Mesh->LoadToGPU() )
		return false;


	ShadowMap = Engine->SpawnObject<BravoShadowMap_Spot>();
	ShadowMap.lock()->Setup(glm::ivec2(2048));

	return true;
}

void BravoSpotLightActor::OnDestroy()
{
	Mesh->ReleaseFromGPU();
	Shader->ReleaseFromGPU();
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
void BravoSpotLightActor::StopUsage()
{
	BravoLightActor::StopUsage();
}