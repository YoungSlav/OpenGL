#include "BravoStaticMeshComponent.h"
#include "BravoActor.h"
#include "BravoLightManager.h"
#include "BravoMaterial.h"
#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoShaderAsset.h"
#include "BravoAssetManager.h"
#include "BravoSelectionManager.h"

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<BravoStaticMeshInstance>("StaticMeshInstance")
        ;
	using namespace rttr;
	registration::class_<BravoStaticMeshComponent>("StaticMeshComponent")
		.property("Instances", &BravoStaticMeshComponent::Instances)
        ;
}

bool BravoStaticMeshComponent::Initialize_Internal()
{
	if ( !BravoComponent::Initialize_Internal() )
		return false;

	if (auto AssetManager = Engine->GetAssetManager())
	{
		SelectionIDShader = AssetManager->FindOrLoad<BravoRenderShaderAsset>("MeshSelectionShader", BravoRenderShaderSettings("MeshSelectionID", false, false));
		OutlineMaskShader = AssetManager->FindOrLoad<BravoRenderShaderAsset>("OutlineMaskShader", BravoRenderShaderSettings("MeshOutlineMask", false, false));
	}

	glGenBuffers(1, &InstancesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, InstancesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &HighlightedInstancesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, HighlightedInstancesSSBO);
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
		Mesh->LoadToGPU();
	}

	if ( !Mesh->EnsureGPUReady() )
		return false;


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

	std::shared_ptr<BravoStaticMeshInstance> newInstance = NewObject<BravoStaticMeshInstance>(
		GetName() + "_instance_" + std::to_string(index),
		InstanceData,
		Self<BravoStaticMeshComponent>(),
		index);

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

bool BravoStaticMeshComponent::IsVisisble() const
{
	if ( std::shared_ptr<BravoActor> owningActor = GetOwningActor() )
		if ( !owningActor->IsVisisble() )
			return false;

	return IBravoRenderable::IsVisisble();
}

void BravoStaticMeshComponent::UpdateHighlightedInstances()
{
	auto SelectionManager = Engine->GetSelectionManager();
	auto Highlights = SelectionManager->GetActiveHighlights();

	auto myHightlights = Highlights.find(Self<BravoStaticMeshComponent>());
	if ( myHightlights == Highlights.end() )
	{
		HighlightedInstancesCount = 0;
		return;
	}

	const std::vector<int32>& HighlightedInstances = myHightlights->second;
	std::vector<BravoInstanceData> HighlightedInstanceData;
	HighlightedInstanceData.reserve(HighlightedInstances.size());
	for ( const int32& i : HighlightedInstances )
		HighlightedInstanceData.push_back(Instances[i]->GetData());

	const std::vector<BravoInstanceData>& HighlightData = HighlightedInstances.size() ? HighlightedInstanceData : InstanceData;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, HighlightedInstancesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, HighlightData.size() * sizeof(BravoInstanceData), HighlightData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	HighlightedInstancesCount = int32(HighlightData.size());

}

void BravoStaticMeshComponent::OnDestroy()
{
	glDeleteBuffers(1, &InstancesSSBO);
	InstancesSSBO = 0;
	
	glDeleteBuffers(1, &HighlightedInstancesSSBO);
	HighlightedInstancesSSBO = 0;

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
		Material->GetShader()->SetFloat3("viewPos", CameraLocation);
		Material->GetShader()->SetFloat1("drawDistance", Engine->GetCamera()->GetMaxDrawingDistance());
		Engine->GetLightManager()->ApplyLights(Material->GetShader());

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, InstancesSSBO);

		Mesh->Render(Instances.size());
		

		Engine->GetLightManager()->ResetLightsUsage();
	Material->StopUsage();
}

void BravoStaticMeshComponent::RenderSelectionID()
{
	if ( !Instances.size() )
		return;

	if ( !EnsureReady() || !SelectionIDShader || !SelectionIDShader->EnsureGPUReady() )
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
		
		Mesh->Render(Instances.size());


	SelectionIDShader->StopUsage();
}

void BravoStaticMeshComponent::RenderOutlineMask()
{
	UpdateHighlightedInstances();

	if ( !HighlightedInstancesCount )
		return;

	if ( !EnsureReady() || !OutlineMaskShader || !OutlineMaskShader->EnsureGPUReady() )
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
		
		OutlineMaskShader->SetFloat1("OutlineColor", (float)(GetHandle()));
		OutlineMaskShader->SetMatrix4d("transform", ModelTranform);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, HighlightedInstancesSSBO);
		
		Mesh->Render(Instances.size());

		
	OutlineMaskShader->StopUsage();
}

void BravoStaticMeshComponent::RenderDepthMap(std::shared_ptr<class BravoShaderAsset> _Shader)
{
	if ( !EnsureReady() )
		return;
	
	glm::mat4 model = GetTransform_World().GetTransformMatrix();

	_Shader->SetMatrix4d("model", model);
		

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, InstancesSSBO);
	Mesh->Render(Instances.size());
}