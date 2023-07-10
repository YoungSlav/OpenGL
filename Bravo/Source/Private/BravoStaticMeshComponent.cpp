#include "BravoStaticMeshComponent.h"
#include "BravoActor.h"
#include "BravoAssetManager.h"
#include "BravoLightManager.h"


bool BravoStaticMeshComponent::Initialize_Internal()
{
	Shader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\Default");
	if ( !Shader )
		return false;
	return true;
}

void BravoStaticMeshComponent::SetMesh(BravoMeshPtr InMesh)
{
	Mesh = InMesh;
}

void BravoStaticMeshComponent::Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	if ( !Mesh || !Shader )
		return;

	glm::mat4 model = GetTransformMatrix_World();

	Shader->Use();
		Shader->SetMatrix4d("projection", CameraProjection);
		Shader->SetMatrix4d("view", CameraView);
		Shader->SetMatrix4d("model", model);
		Shader->SetMaterial("material", Mesh->GetMaterial());
		Shader->SetVector3d("viewPos", CameraLocation);
		Engine->GetLightManager()->ApplyLights(Shader);
			Mesh->Render();
		Engine->GetLightManager()->ResetLightsUsage();
	Shader->StopUsage();
}

void BravoStaticMeshComponent::RenderDepthMap(std::shared_ptr<class BravoShader> _Shader) const
{
	if ( !Mesh || !_Shader)
		return;
	
	glm::mat4 model = GetTransformMatrix_World();

	_Shader->Use();
		_Shader->SetMatrix4d("model", model);
		Mesh->Render();
	_Shader->StopUsage();
}