#include "BravoMeshActor.h"
#include "BravoViewport.h"
#include "BravoEngine.h"
#include "BravoLightManager.h"

void BravoMeshActor::Init()
{
	Shader = BravoAsset::Load<BravoShader>("Default");
}

void BravoMeshActor::SetMesh(BravoMeshPtr InMesh)
{
	Mesh = InMesh;
}

void BravoMeshActor::Tick(float DeltaTime)
{
}

void BravoMeshActor::Draw(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	if ( Mesh && Shader )
	{
		glm::mat4 model = TransformModelMatrix();
		std::vector<std::shared_ptr<BravoMeshPart>>	MeshParts = Mesh->GetMeshParts();
		for ( std::shared_ptr<BravoMeshPart>& part : MeshParts )
		{
			if ( part->VAO != -1 )
			{
				Shader->Use();
				Shader->SetMatrix4d("projection", CameraProjection);
				Shader->SetMatrix4d("view", CameraView);
				Shader->SetMatrix4d("model", model);
					
				Shader->SetMaterial("material", part->Material);
				Shader->SetVector3d("viewPos", CameraLocation);

				Engine->GetViewport()->GetLightManager()->ApplyLights(Shader);
        
				// draw mesh
				glBindVertexArray(part->VAO);
				glDrawElements(GL_TRIANGLES, part->Indices.size(), GL_UNSIGNED_INT, 0);

				glBindVertexArray(0);
				glActiveTexture(0);

				if ( part->Material )
					part->Material->StopUsage();

				Engine->GetViewport()->GetLightManager()->ResetLightsUsage();
				Shader->StopUsage();
			}
		}
	}
}

void BravoMeshActor::DrawToShadowMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& Position) const
{
	if ( Mesh && Shader)
	{
		std::vector<std::shared_ptr<BravoMeshPart>>	MeshParts = Mesh->GetMeshParts();
		glm::mat4 model = TransformModelMatrix();
		for ( std::shared_ptr<BravoMeshPart>& part : MeshParts )
		{
			if ( part->VAO != -1 )
			{
				Shader->SetMatrix4d("model", model);
				glBindVertexArray(part->VAO);
					glDrawElements(GL_TRIANGLES, part->Indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}
	}
}

void BravoMeshActor::OnDestroy()
{
	Mesh->UnLoad();
	Shader->UnLoad();
}