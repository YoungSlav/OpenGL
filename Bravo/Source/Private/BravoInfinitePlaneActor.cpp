#include "BravoInfinitePlaneActor.h"
#include "BravoEngine.h"
#include "BravoCamera.h"
#include "BravoAssetManager.h"
#include "BravoStaticMeshAsset.h"

bool BravoInfinitePlaneActor::Initialize_Internal()
{
	Mesh = Engine->GetAssetManager()->FindOrLoad<BravoStaticMeshAsset>("PlaneMeshAsset", BravoStaticMeshSettings("primitives\\plane.fbx"));
	Shader = Engine->GetAssetManager()->FindOrLoad<BravoRenderShaderAsset>("InfinitePlaneShader", BravoRenderShaderSettings("InfinitePlane", false, false));

	if ( Mesh && Shader )
	{
		Mesh->LoadToGPU();
		Shader->LoadToGPU();
		return true;
	}
	return false;
}

bool BravoInfinitePlaneActor::EnsureReady()
{
	if ( !Mesh || !Shader )
		return false;

	if ( !Shader->EnsureGPUReady() )
		return false;

	if ( Mesh->GetLoadingState() == EAssetLoadingState::InRAM )
	{
		Mesh->LoadToGPU();
	}
	if ( !Mesh->EnsureGPUReady() )
		return false;

	return true;
}

void BravoInfinitePlaneActor::Render()
{
	if ( !EnsureReady() )
		return;

	glm::mat4 CameraProjection;
	glm::mat4 CameraView;
	float MinDrawingDistance;
	float MaxDrawingDistance;
	if ( std::shared_ptr<BravoCamera> camera = Engine->GetCamera() )
	{
		CameraProjection = camera->GetProjectionMatrix();
		CameraView = camera->GetViewMatrix();
		MinDrawingDistance = camera->GetMinDrawingDistance();
		MaxDrawingDistance = camera->GetMaxDrawingDistance();
	}
	else
	{
		return;
	}

	glm::mat4 model = GetTransform_World().GetTransformMatrix();
	Shader->Use();
		Shader->SetMatrix4d("projection", CameraProjection);
		Shader->SetMatrix4d("view", CameraView);
		Shader->SetFloat1("near", MinDrawingDistance);
		Shader->SetFloat1("far", MaxDrawingDistance);

		
		Mesh->Render(1);

	Shader->StopUsage();
}

void BravoInfinitePlaneActor::OnDestroy()
{
	Shader->ReleaseFromGPU();
	Mesh->ReleaseFromGPU();
}