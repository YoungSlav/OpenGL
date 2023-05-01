#include "BravoInfinitePlaneActor.h"
#include "BravoEngine.h"
#include "BravoCamera.h"

void BravoInfinitePlaneActor::Init()
{
	
	Mesh = BravoAsset::Load<BravoMesh>("primitives\\plane.fbx");
	Shader = BravoAsset::Load<BravoShader>("InfinitePlane");
}

void BravoInfinitePlaneActor::Draw(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	if ( Mesh )
	{
		std::vector<std::shared_ptr<BravoMeshPart>>	MeshParts = Mesh->GetMeshParts();
		for ( std::shared_ptr<BravoMeshPart>& part : MeshParts )
		{
			glm::mat4 model = TransformModelMatrix();
	
			if ( Shader )
			{
				if ( part->VAO != -1 )
				{
					Shader->Use();
					Shader->SetMatrix4d("projection", CameraProjection);
					Shader->SetMatrix4d("view", CameraView);
					Shader->SetVector1d("near", GetEngine()->GetCamera()->GetMinDrawingDistance());
					Shader->SetVector1d("far", GetEngine()->GetCamera()->GetMaxDrawingDistance());
        
					// draw mesh
					glBindVertexArray(part->VAO);
						glDrawElements(GL_TRIANGLES, part->Indices.size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
					glActiveTexture(0);

					
					Shader->StopUsage();
				}
			}
		}
	}
}

void BravoInfinitePlaneActor::OnDestroy()
{
	Shader->UnLoad();
	Mesh->UnLoad();
}