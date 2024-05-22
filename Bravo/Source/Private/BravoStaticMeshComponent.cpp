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

	glGenBuffers(1, &InstancesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, InstancesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &SelectedInstancesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SelectedInstancesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

void BravoStaticMeshComponent::OnInstanceTransformUpdated(const IBravoTransformable* inst)
{
	if ( const BravoStaticMeshInstance* asInst = static_cast<const BravoStaticMeshInstance*>(inst) )
	{
		UpdateInstance(asInst->InstanceIndex, asInst->GetData());
	}
}

int32 BravoStaticMeshComponent::AddInstance(const BravoInstanceData& InstanceData)
{
	int32 index = (int32)Instances.size();

	std::shared_ptr<BravoStaticMeshInstance> newInstance(
			new BravoStaticMeshInstance(
				InstanceData,
				Self<BravoStaticMeshComponent>(),
				index));

	newInstance->OnTransformUpdated.AddSP(Self<BravoStaticMeshComponent>(), &BravoStaticMeshComponent::OnInstanceTransformUpdated);

	Instances.push_back(newInstance);

	bInstanceStateDirty = true;

	return index;
}

void BravoStaticMeshComponent::UpdateInstance(int32 Index, const BravoInstanceData& NewData)
{
	if (Index < 0 || Index >= Instances.size())
		return;
	Instances[Index]->SetData(NewData);
	InstanceData[Index] = NewData;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, InstancesSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, Index * sizeof(BravoInstanceData), sizeof(BravoInstanceData), &InstanceData[Index]);
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

std::shared_ptr<BravoStaticMeshInstance> BravoStaticMeshComponent::GetInstance(int32 Index) const
{
	if ( Index >= 0 && Index < InstanceCount() )
	{
		return Instances[Index];
	}
	return nullptr;
}

void BravoStaticMeshComponent::UpdateInstanceBuffer()
{
	InstanceData.clear();
	InstanceData.reserve(Instances.size());
	for ( auto it : Instances )
	{
		InstanceData.push_back(it->GetData());
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, InstancesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, InstanceData.size() * sizeof(BravoInstanceData), InstanceData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	bInstanceStateDirty = false;
}

void BravoStaticMeshComponent::SetSelection(const std::vector<int32>& SelectedInstances)
{
	std::vector<BravoInstanceData> SelectedInstanceData;
	SelectedInstanceData.reserve(SelectedInstances.size());
	for ( const int32& i : SelectedInstances )
	{
		SelectedInstanceData.push_back(Instances[i]->GetData());
	}

	const std::vector<BravoInstanceData>& selectedData = SelectedInstances.size() ? SelectedInstanceData : InstanceData;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SelectedInstancesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, selectedData.size() * sizeof(BravoInstanceData), selectedData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	SelectedInstancesCount = (int32)selectedData.size();
}

void BravoStaticMeshComponent::ClearSelection()
{
	SelectedInstancesCount = 0;
}

void BravoStaticMeshComponent::OnDestroy()
{
	glDeleteBuffers(1, &InstancesSSBO);
	InstancesSSBO = 0;
	
	glDeleteBuffers(1, &SelectedInstancesSSBO);
	SelectedInstancesSSBO = 0;

	glDeleteVertexArrays(6, &VAO);
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

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, InstancesSSBO);

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

		
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, InstancesSSBO);
		
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0, (int32)Instances.size());
		glBindVertexArray(0);


	SelectionIDShader->StopUsage();
}

void BravoStaticMeshComponent::RenderOutlineMask()
{
	if ( SelectedInstancesCount == 0)
		return;

	if ( !EnsureReady() || !OutlineMaskShader || !OutlineMaskShader->EnsureReady() )
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	glm::mat4 CameraView = camera->GetViewMatrix();
	glm::vec3 CameraLocation = camera->GetLocation();

	glm::mat4 model = GetTransform_World().GetTransformMatrix();

	glm::mat4 ModelTranform = CameraProjection * CameraView * model;

	OutlineMaskShader->Use();
		
		OutlineMaskShader->SetVector1d("OutlineColor", (float)(GetHandle()));
		OutlineMaskShader->SetMatrix4d("transform", ModelTranform);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SelectedInstancesSSBO);
		
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0, SelectedInstancesCount);
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

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, InstancesSSBO);
	glDrawElementsInstanced(GL_TRIANGLES, (int32)Mesh->GetIndices().size(), GL_UNSIGNED_INT, 0, (int32)Instances.size());
}