#include "BravoInfinitePlaneActor.h"
#include "BravoEngine.h"
#include "BravoCamera.h"
#include "BravoAssetManager.h"

void BravoInfinitePlaneActor::Init()
{
	Mesh = GetEngine()->GetAssetManager()->LoadAsset<BravoMesh>("primitives\\plane.fbx");
	Shader = GetEngine()->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\InfinitePlane");

	if ( Mesh && Shader )
	{
	Mesh->LoadToGPU();
	Shader->LoadToGPU();
	}
}

void BravoInfinitePlaneActor::Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	if ( Mesh && Shader )
	{
		glm::mat4 model = GetTransformMatrix();
		Shader->Use();
			Shader->SetMatrix4d("projection", CameraProjection);
			Shader->SetMatrix4d("view", CameraView);
			Shader->SetVector1d("near", GetEngine()->GetCamera()->GetMinDrawingDistance());
			Shader->SetVector1d("far", GetEngine()->GetCamera()->GetMaxDrawingDistance());

			Mesh->Render();
		Shader->StopUsage();
	}
}

void BravoInfinitePlaneActor::OnDestroy()
{
	Shader->ReleaseFromGPU();
	Mesh->ReleaseFromGPU();
}