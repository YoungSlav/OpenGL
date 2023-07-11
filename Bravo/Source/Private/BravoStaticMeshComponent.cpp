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

bool BravoStaticMeshComponent::EnsureReady()
{
	if ( !Mesh || !Shader || !Material )
		return false;

	if ( !Mesh->IsLoadedToGPU() )
	{
		if ( VAO != 0 )
			glDeleteVertexArrays(5, &VAO);
		VAO = 0;
	}

	if ( !Mesh->EnsureReady() )
		return false;

	if ( VAO == 0 )
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,  Mesh->GetVBO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh->GetEBO());

		// vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Normal));
		glEnableVertexAttribArray(1);
		// vertex texture coords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TexCoords));
		glEnableVertexAttribArray(2);
		// vertex tangent
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Tangent));
		glEnableVertexAttribArray(3);
		// vertex bitangent
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Bitangent));
		glEnableVertexAttribArray(4);
		// vertex colors
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Color));
		glEnableVertexAttribArray(5);
	}

	return true;
}

void BravoStaticMeshComponent::OnDestroy()
{
	glDeleteVertexArrays(5, &VAO);
	VAO = 0;
	Shader->ReleaseFromGPU();
	Mesh->ReleaseFromGPU();
}

void BravoStaticMeshComponent::SetMesh(BravoMeshPtr InMesh)
{
	Mesh = InMesh;
}

BravoMaterialPtr BravoStaticMeshComponent::GetMaterial() const
{
	return Material;
}
void BravoStaticMeshComponent::SetMaterial(BravoMaterialPtr _Material)
{
	if ( Material )
		Material->StopUsage();
	Material = _Material;
}

void BravoStaticMeshComponent::Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView)
{
	if ( !EnsureReady() )
		return;

	glm::mat4 model = GetTransformMatrix_World();

	Shader->Use();
		Material->Use();
		Shader->SetMatrix4d("projection", CameraProjection);
		Shader->SetMatrix4d("view", CameraView);
		Shader->SetMatrix4d("model", model);
		Shader->SetMaterial("material", Material);
		Shader->SetVector3d("viewPos", CameraLocation);
		Engine->GetLightManager()->ApplyLights(Shader);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glActiveTexture(0);


		Engine->GetLightManager()->ResetLightsUsage();
		Material->StopUsage();
	Shader->StopUsage();
}

void BravoStaticMeshComponent::RenderDepthMap(std::shared_ptr<class BravoShader> _Shader)
{
	if ( !EnsureReady() )
		return;
	
	glm::mat4 model = GetTransformMatrix_World();

	_Shader->Use();
		_Shader->SetMatrix4d("model", model);
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0);
		glActiveTexture(0);

	_Shader->StopUsage();
}