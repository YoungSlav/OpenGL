#include "BravoStaticMeshComponent.h"
#include "BravoActor.h"
#include "BravoLightManager.h"
#include "BravoMaterial.h"
#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoShaderAsset.h"
#include "BravoAssetManager.h"



bool BravoStaticMeshComponent::Initialize_Internal()
{
	if ( !BravoComponent::Initialize_Internal() )
		return false;

	if (auto AssetManager = Engine->GetAssetManager())
	{
		SelectionIDShader = AssetManager->FindOrLoad<BravoShaderAsset>("MeshSelectionShader", BravoShaderLoadingParams("Shaders\\MeshSelectionID"));
		OutlineMaskShader = AssetManager->FindOrLoad<BravoShaderAsset>("OutlineMaskShader", BravoShaderLoadingParams("Shaders\\MeshOutlineMask"));
	}

	AddInstance(BravoInstanceData());
	return true;
}

bool BravoStaticMeshComponent::EnsureReady()
{
	if ( !Mesh )
		return false;

	if ( Mesh->GetLoadingState() == EAssetLoadingState::InRAM )
	{
		if ( VAO != 0 )
			glDeleteVertexArrays(10, &VAO);
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

		glBindVertexArray(0);
	}

	if ( bInstanceStateDirty )
		UpdateInstanceBuffer();

	return true;
}

void BravoStaticMeshComponent::OnInstanceTransformUpdated(const ITransformable* inst)
{
	if ( const BravoStaticMeshComponent::Instance* asInst = static_cast<const BravoStaticMeshComponent::Instance*>(inst) )
	{
		UpdateInstance(asInst->InstanceIndex, asInst->GetData());
	}
}

int32 BravoStaticMeshComponent::AddInstance(const BravoInstanceData& InstanceData)
{
	int32 index = (int32)Instances.size();

	std::shared_ptr<BravoStaticMeshComponent::Instance> newInstance(
			new BravoStaticMeshComponent::Instance(
				InstanceData,
				Self<BravoStaticMeshComponent>(),
				index));

	newInstance->OnTransformUpdated.AddSP(Self<BravoStaticMeshComponent>(), &BravoStaticMeshComponent::OnInstanceTransformUpdated);

	Instances.push_back(newInstance);

	bInstanceStateDirty = true;

	return index;
}

void BravoStaticMeshComponent::UpdateInstance(int32 Index, const BravoInstanceData& InstanceData)
{
	if (Index < 0 || Index >= Instances.size())
		return;
	Instances[Index]->SetData(InstanceData);
	bInstanceStateDirty = true;
}

void BravoStaticMeshComponent::RemoveAllInstances()
{
	Instances.clear();
	bInstanceStateDirty = true;
}

void BravoStaticMeshComponent::RemoveInstances(int32 Index, int32 Count)
{
	Instances.erase(Instances.begin() + Index, Instances.begin() + Index + Count);
	bInstanceStateDirty = true;
}

BravoInstanceData BravoStaticMeshComponent::GetInstanceData(int32 Index) const
{
	if ( Index >= 0 && Index < InstanceCount() )
	{
		return Instances[Index]->GetData();
	}
	return BravoInstanceData();
}

void BravoStaticMeshComponent::UpdateInstanceBuffer()
{
	// TODO
	


	bInstanceStateDirty = false;
	
}

void BravoStaticMeshComponent::OnDestroy()
{
	glDeleteVertexArrays(10, &VAO);
	VAO = 0;
	Material->Destroy();
	Mesh->ReleaseFromGPU();
}

void BravoStaticMeshComponent::SetMesh(std::shared_ptr<BravoStaticMeshAsset> InMesh)
{
	Mesh = InMesh;
}

std::shared_ptr<BravoMaterial> BravoStaticMeshComponent::GetMaterial() const
{
	return Material;
}
void BravoStaticMeshComponent::SetMaterial(std::shared_ptr<BravoMaterial> _Material)
{
	if ( Material )
		Material->Destroy();
	Material = _Material;
}

void BravoStaticMeshComponent::Render()
{
	if ( !Instances.size() )
		return;

	if ( !EnsureReady() || !Material || !Material->EnsureReady() )
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	const glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	const glm::mat4 CameraView = camera->GetViewMatrix();
	const glm::vec3 CameraLocation = camera->GetLocation();
	
	const glm::mat4 model = GetTransform_World().GetTransformMatrix();

	Material->Use();
		Material->GetShader()->SetMatrix4d("projection", CameraProjection);
		Material->GetShader()->SetMatrix4d("view", CameraView);
		Material->GetShader()->SetMatrix4d("model", model);
		Material->GetShader()->SetVector3d("viewPos", CameraLocation);
		Material->GetShader()->SetVector1d("drawDistance", Engine->GetCamera()->GetMaxDrawingDistance());
		Engine->GetLightManager()->ApplyLights(Material->GetShader());

		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0, (int32)Instances.size());
		glBindVertexArray(0);


		Engine->GetLightManager()->ResetLightsUsage();
	Material->StopUsage();
}

void BravoStaticMeshComponent::RenderSelectionID()
{
	if ( !Instances.size() )
		return;

	if ( !EnsureReady() || !SelectionIDShader || !SelectionIDShader->EnsureReady() )
		return;

	std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	glm::mat4 CameraView = camera->GetViewMatrix();
	glm::vec3 CameraLocation = camera->GetLocation();

	glm::mat4 model = GetTransform_World().GetTransformMatrix();

	glm::mat4 ModelTranform = CameraProjection * CameraView * model;
	SelectionIDShader->Use();
		SelectionIDShader->SetMatrix4d("transform", ModelTranform);
		SelectionIDShader->SetInt("ObjectHandle", GetHandle());
		
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0, (int32)Instances.size());
		glBindVertexArray(0);

	SelectionIDShader->StopUsage();
}

void BravoStaticMeshComponent::RenderOutlineMask(int32 InstanceID)
{
	if ( !Instances.size() )
		return;

	if ( !EnsureReady() || !OutlineMaskShader || !OutlineMaskShader->EnsureReady() )
		return;

	if ( Instances.size() <= InstanceID || InstanceID < 0  )
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	glm::mat4 CameraView = camera->GetViewMatrix();
	

	const glm::mat4 model = GetTransform_World().GetTransformMatrix();
	const glm::mat4 InstanceTransorm = CameraProjection * CameraView * model * Instances[InstanceID]->GetTransform().GetTransformMatrix();
	OutlineMaskShader->Use();
		
		OutlineMaskShader->SetVector1d("OutlineColor", (float)(GetHandle()));
		OutlineMaskShader->SetMatrix4d("transform", InstanceTransorm);
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
	OutlineMaskShader->StopUsage();
}

void BravoStaticMeshComponent::RenderDepthMap(std::shared_ptr<class BravoShaderAsset> _Shader)
{
	if ( !EnsureReady() )
		return;
	
	glm::mat4 model = GetTransform_World().GetTransformMatrix();

	_Shader->SetMatrix4d("model", model);
		
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0, (int32)Instances.size());

}