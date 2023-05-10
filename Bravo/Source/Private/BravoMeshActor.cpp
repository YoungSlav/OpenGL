#include "BravoMeshActor.h"

#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoLightManager.h"

void BravoMeshActor::Initialize_Internal()
{
	Shader = GetEngine()->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\Default");
}

void BravoMeshActor::SetMesh(BravoMeshPtr InMesh)
{
	Mesh = InMesh;
}

void BravoMeshActor::Tick(float DeltaTime)
{
}

void BravoMeshActor::Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	if ( Mesh && Shader )
	{
		glm::mat4 model = GetTransformMatrix();

		Shader->Use();
			Shader->SetMatrix4d("projection", CameraProjection);
			Shader->SetMatrix4d("view", CameraView);
			Shader->SetMatrix4d("model", model);
			Shader->SetMaterial("material", Mesh->GetMaterial());
			Shader->SetVector3d("viewPos", CameraLocation);
			GetEngine()->GetLightManager()->ApplyLights(Shader);
			Mesh->Render();
			GetEngine()->GetLightManager()->ResetLightsUsage();
		Shader->StopUsage();
	}
}

void BravoMeshActor::RenderDepthMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& LightPosition) const
{
	if ( Mesh && Shader)
	{
		glm::mat4 model = GetTransformMatrix();

		Shader->Use();
			Shader->SetMatrix4d("model", model);
			Mesh->Render();
		Shader->StopUsage();
	}
}
