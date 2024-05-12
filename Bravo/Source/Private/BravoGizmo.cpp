#include "BravoGizmo.h"
#include "IBravoRenderable.h"
#include "BravoStaticMeshComponent.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"
#include "BravoMaterialUnlit.h"
#include "BravoInput.h"

bool BravoGizmo::Initialize_Internal()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	BravoUnlitMaterialParams materailLoadingParams;
	materailLoadingParams.bUseVertexColor = false;

	glm::vec3 Rotation = glm::vec3(0.0f);


	// TRANFRORM
	auto TransformMesh = AssetManager->FindOrLoad<BravoStaticMeshAsset>("GizmoTrAsset", BravoStaticMeshLoadingParams("Gizmo\\SM_Arrow_01.fbx"));
	{
	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 0.0, 0.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 0.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("TransformX");
	Mesh->SetMesh(TransformMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	TransformComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 0.0, 1.0);
	glm::vec3 Rotation = glm::vec3(0.0f, -90.0, 0.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("TransformY");
	Mesh->SetMesh(TransformMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	TransformComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 1.0, 0.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 90.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("TransformZ");
	Mesh->SetMesh(TransformMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	TransformComponents.push_back(Mesh);
	}
	
	// SCALE
	auto ScaleMesh = AssetManager->FindOrLoad<BravoStaticMeshAsset>("GizmoScAsset", BravoStaticMeshLoadingParams("Gizmo\\SM_Scale.fbx"));
	{
	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 0.0, 0.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 0.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("ScaleX");
	Mesh->SetMesh(ScaleMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(4.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	ScaleComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 0.0, 1.0);
	glm::vec3 Rotation = glm::vec3(0.0f, -90.0, 0.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("ScaleY");
	Mesh->SetMesh(ScaleMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(4.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	ScaleComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 1.0, 0.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 90.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("ScaleZ");
	Mesh->SetMesh(ScaleMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(4.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	ScaleComponents.push_back(Mesh);
	}

	// ROTATION
	auto RotationMesh = AssetManager->FindOrLoad<BravoStaticMeshAsset>("GizmoRotAsset", BravoStaticMeshLoadingParams("Gizmo\\SM_Angle_Smooth.fbx"));
	{
	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 0.0, 0.0);
	glm::vec3 Rotation = glm::vec3(90.0f, -90.0, 90.0f);

	auto Mesh = NewObject<BravoStaticMeshComponent>("RotationX");
	Mesh->SetMesh(RotationMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(4.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	RotationComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 1.0, 0.0);
	glm::vec3 Rotation = glm::vec3(90.0f, -90.0, 0.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("RotationY");
	Mesh->SetMesh(RotationMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(4.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	RotationComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 0.0, 1.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 90.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("RotationZ");
	Mesh->SetMesh(RotationMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(4.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	RotationComponents.push_back(Mesh);
	}

	SetGizmoState(GizmoState);

	return true;
}

void BravoGizmo::OnDestroy()
{
	TransformComponents.clear();
	RotationComponents.clear();
	ScaleComponents.clear();
}

void BravoGizmo::OnBeginPlay()
{
	if ( std::shared_ptr<BravoInput> Input = Engine->GetInput() )
	{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_SPACE;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<BravoGizmo>(), &BravoGizmo::OnInput_ChangeGizmo);
		Input->SubscribeKey(subscription);
	}
}

void BravoGizmo::OnInput_ChangeGizmo(bool ButtonState, float DeltaTime)
{
	int32 CurrentState = (int32)(GizmoState);
	int32 NewState = (CurrentState+1) % 3;
	SetGizmoState(EBravoGizmoState(NewState));
}

void BravoGizmo::SetGizmoState(EBravoGizmoState NewState)
{
	GizmoState = NewState;
	
	for ( auto it : TransformComponents )
		it->SetVisisble(GizmoState == EBravoGizmoState::Transform);
	for ( auto it : RotationComponents)
		it->SetVisisble(GizmoState == EBravoGizmoState::Rotation);
	for ( auto it : ScaleComponents)
		it->SetVisisble(GizmoState == EBravoGizmoState::Scale);
}