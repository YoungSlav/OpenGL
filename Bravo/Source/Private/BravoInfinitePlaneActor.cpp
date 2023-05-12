#include "BravoInfinitePlaneActor.h"
#include "BravoEngine.h"
#include "BravoCamera.h"
#include "BravoAssetManager.h"

bool BravoInfinitePlaneActor::Initialize_Internal()
{
	Mesh = Engine->GetAssetManager()->LoadAsset<BravoMesh>("primitives\\plane.fbx");
	Shader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\InfinitePlane");

	if ( Mesh && Shader )
	{
		Mesh->LoadToGPU();
		Shader->LoadToGPU();
		return true;
	}
	return false;
}

void BravoInfinitePlaneActor::Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	if ( Mesh && Shader )
	{
		glm::mat4 model = GetTransformMatrix();
		Shader->Use();
			Shader->SetMatrix4d("projection", CameraProjection);
			Shader->SetMatrix4d("view", CameraView);
			Shader->SetVector1d("near", Engine->GetCamera()->GetMinDrawingDistance());
			Shader->SetVector1d("far", Engine->GetCamera()->GetMaxDrawingDistance());

			Mesh->Render();
		Shader->StopUsage();
	}
}

void BravoInfinitePlaneActor::OnDestroy()
{
	Shader->ReleaseFromGPU();
	Mesh->ReleaseFromGPU();
}