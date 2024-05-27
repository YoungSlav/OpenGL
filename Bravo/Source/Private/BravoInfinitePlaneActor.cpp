#include "BravoInfinitePlaneActor.h"
#include "BravoEngine.h"
#include "BravoCamera.h"
#include "BravoAssetManager.h"
#include "BravoStaticMeshAsset.h"

bool BravoInfinitePlaneActor::Initialize_Internal()
{
	Mesh = Engine->GetAssetManager()->FindOrLoad<BravoStaticMeshAsset>("PlaneMeshAsset", BravoStaticMeshLoadingParams("primitives\\plane.fbx"));
	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("InfinitePlaneShader", BravoShaderLoadingParams("Shaders\\InfinitePlane"));

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

	if ( Mesh->GetLoadingState() == EAssetLoadingState::InRAM )
	{
		if ( VAO != 0 )
			glDeleteVertexArrays(5, &VAO);
		VAO = 0;
	}
	if ( !Mesh->EnsureGPUReady() )
		return false;

	if ( VAO == 0 )
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, Mesh->GetVBO());
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
		Shader->SetVector1d("near", MinDrawingDistance);
		Shader->SetVector1d("far", MaxDrawingDistance);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0);

	Shader->StopUsage();
}

void BravoInfinitePlaneActor::OnDestroy()
{
	glDeleteBuffers(5, &VAO);
	VAO = 0;

	Shader->ReleaseFromGPU();
	Mesh->ReleaseFromGPU();
}