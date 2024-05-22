#include "BravoGizmo.h"
#include "IBravoRenderable.h"
#include "BravoStaticMeshComponent.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"
#include "BravoMaterialUnlit.h"
#include "BravoInput.h"
#include "BravoViewport.h"
#include "IBravoTransformable.h"
#include "BravoCamera.h"

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
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("TransformX", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(TransformMesh);
	Mesh->SetScale(glm::vec3(0.25f));
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnTransformX);
	TransformComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 0.0, 1.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 90.0, 0.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("TransformZ", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(TransformMesh);
	Mesh->SetScale(glm::vec3(0.25f));
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnTransformZ);
	TransformComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 1.0, 0.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 90.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("TransformY", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(TransformMesh);
	Mesh->SetScale(glm::vec3(0.25f));
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnTransformY);
	TransformComponents.push_back(Mesh);
	}
	
	// SCALE
	auto ScaleMesh = AssetManager->FindOrLoad<BravoStaticMeshAsset>("GizmoScAsset", BravoStaticMeshLoadingParams("Gizmo\\SM_Scale.fbx"));
	{
	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 0.0, 0.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 0.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("ScaleX", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(ScaleMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(1.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnScaleX);
	ScaleComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 0.0, 1.0);
	glm::vec3 Rotation = glm::vec3(0.0f, -90.0, 0.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("ScaleZ", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(ScaleMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(1.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnScaleZ);
	ScaleComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 1.0, 0.0);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 90.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("ScaleY", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(ScaleMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(1.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnScaleY);
	ScaleComponents.push_back(Mesh);
	}

	// ROTATION
	auto RotationMesh = AssetManager->FindOrLoad<BravoStaticMeshAsset>("GizmoRotAsset", BravoStaticMeshLoadingParams("Gizmo\\SM_Angle_Smooth.fbx"));
	{
	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 0.0, 0.0);

	auto Mesh = NewObject<BravoStaticMeshComponent>("RotationX", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(RotationMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetScale(glm::vec3(1.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnRotationX);
	
	Mesh->RemoveAllInstances();
	BravoTransform transform;
	transform.SetRotation(glm::vec3(90.0f, 0.0, 90.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(90.0f, 90.0, 90.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(90.0f, 180.0, 90.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(90.0f, 270.0, 90.0f));
	Mesh->AddInstance(BravoInstanceData(transform));

	RotationComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 1.0, 0.0);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("RotationY", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(RotationMesh);
	Mesh->SetCastShadows(false);
	Mesh->SetScale(glm::vec3(1.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnRotationY);

	Mesh->RemoveAllInstances();
	BravoTransform transform;
	transform.SetRotation(glm::vec3(90.0f, 0.0, 0.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(90.0f, 90.0, 0.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(90.0f, 180.0, 0.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(90.0f, 270.0, 0.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	RotationComponents.push_back(Mesh);
	}
	{
	materailLoadingParams.AlbedoColor = glm::vec3(0.0f, 0.0, 1.0);
	//glm::vec3 Rotation = glm::vec3(0.0f, 0.0, 90.0f);
	
	auto Mesh = NewObject<BravoStaticMeshComponent>("RotationZ", ERenderPriority::Starndart, ERenderGroup::Overlay);
	Mesh->SetMesh(RotationMesh);
	Mesh->SetCastShadows(false);
	//Mesh->SetRotation(Rotation);
	Mesh->SetScale(glm::vec3(1.0f));
	auto material = Mesh->NewObject<BravoMaterialUnlit>();
	material->Load(materailLoadingParams);
	Mesh->SetMaterial(material);
	Mesh->OnObjectClicked.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnRotationZ);
	
	Mesh->RemoveAllInstances();
	BravoTransform transform;
	transform.SetRotation(glm::vec3(0.0f, 0.0, 0.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(0.0f, 0.0, 90.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(0.0f, 0.0, 180.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	transform.SetRotation(glm::vec3(0.0f, 0.0, 270.0f));
	Mesh->AddInstance(BravoInstanceData(transform));
	
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
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_SPACE;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<BravoGizmo>(), &BravoGizmo::OnInput_ChangeGizmo);
		Input->SubscribeKey(subscription);
		}

		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_MOUSE_BUTTON_LEFT;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<BravoGizmo>(), &BravoGizmo::OnInput_MouseReleased);
		Input->SubscribeKey(subscription);
		}

		Input->OnMouseMoveDelegate.AddSP(Self<BravoGizmo>(), &BravoGizmo::OnMouseMove);
	}
}

void BravoGizmo::Tick(float DeltaTime)
{
	std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	const glm::ivec2 ViewportSize = Engine->GetViewport()->GetViewportSize();

	float desiredNDCSizeX = (DesiredScreenSize / ViewportSize.x) * 2.0f;
	float desiredNDCSizeY = (DesiredScreenSize / ViewportSize.y) * 2.0f;

	float desiredNDCSize = glm::max(desiredNDCSizeX, desiredNDCSizeY);

	float distance = glm::distance(GetLocation(), camera->GetLocation_World());

	float halfFovY = camera->GetFOV() / 2.0f;
    float screenHeightAtGizmo = 2.0f * distance * tan(halfFovY);
    float scaleFactor = DesiredScreenSize * (screenHeightAtGizmo / ViewportSize.y);

	SetScale(glm::vec3(scaleFactor));
}

void BravoGizmo::SetGizmoState(EBravoGizmoState NewState)
{
	ResetInput();
	GizmoState = NewState;
	
	for ( auto it : TransformComponents )
		it->SetVisisble(GizmoState == EBravoGizmoState::Transform);
	for ( auto it : RotationComponents)
		it->SetVisisble(GizmoState == EBravoGizmoState::Rotation);
	for ( auto it : ScaleComponents)
		it->SetVisisble(GizmoState == EBravoGizmoState::Scale);
}

void BravoGizmo::OnInput_ChangeGizmo(bool ButtonState, float DeltaTime)
{
	int32 CurrentState = (int32)(GizmoState);
	int32 NewState = (CurrentState+1) % 3;
	SetGizmoState(EBravoGizmoState(NewState));
}

const glm::vec3& BravoGizmo::SelectPlane(const glm::vec3& NormalA, const glm::vec3& NormalB) const
{
	std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return NormalA;

	const glm::vec3 cameraDirection = camera->GetForwardVector_World();

	const float dotA = std::abs(glm::dot(cameraDirection, NormalA));
	const float dotB = std::abs(glm::dot(cameraDirection, NormalB));

	return dotA > dotB ? NormalA : NormalB;
}

void BravoGizmo::OnTransformX(const int32&)
{
	ResetInput();
	InputMask = BravoMath::rightV;
	InputPlane = SelectPlane(BravoMath::forwardV, BravoMath::upV);
	
	bInputActive = CastRay(OldIntersection);
}
void BravoGizmo::OnTransformY(const int32&)
{
	ResetInput();
	InputMask = BravoMath::upV;
	InputPlane = SelectPlane(BravoMath::forwardV, BravoMath::rightV);
	
	bInputActive = CastRay(OldIntersection);
}
void BravoGizmo::OnTransformZ(const int32&)
{
	ResetInput();
	InputMask = -BravoMath::forwardV;
	InputPlane = SelectPlane(BravoMath::rightV, BravoMath::upV);

	bInputActive = CastRay(OldIntersection);
}

void BravoGizmo::OnScaleX(const int32&)
{
	ResetInput();
	InputMask = BravoMath::rightV;
	InputPlane = SelectPlane(BravoMath::forwardV, BravoMath::upV);

	bInputActive = CastRay(OldIntersection);
	glm::vec3 InputOffset = GetLocation_World() - OldIntersection;

	if ( BravoMath::IsNearlyZero(BravoMath::MaxComponent(InputOffset * InputMask)) )
		ResetInput();
}
void BravoGizmo::OnScaleY(const int32&)
{
	ResetInput();
	InputMask = BravoMath::upV;
	InputPlane = SelectPlane(BravoMath::forwardV, BravoMath::rightV);

	bInputActive = CastRay(OldIntersection);
	glm::vec3 InputOffset = GetLocation_World() - OldIntersection;

	if ( BravoMath::IsNearlyZero(BravoMath::MaxComponent(InputOffset * InputMask)) )
		ResetInput();
}
void BravoGizmo::OnScaleZ(const int32&)
{
	ResetInput();
	InputMask = -BravoMath::forwardV;
	InputPlane = SelectPlane(BravoMath::rightV, BravoMath::upV);

	bInputActive = CastRay(OldIntersection);
	glm::vec3 InputOffset = GetLocation_World() - OldIntersection;

	if ( BravoMath::IsNearlyZero(BravoMath::MaxComponent(InputOffset * InputMask)) )
		ResetInput();
}

void BravoGizmo::OnRotationX(const int32&)
{
	ResetInput();
	InputMask = GetRightVector_World();
	InputPlane = GetRightVector_World();

	bInputActive = CastRay(OldIntersection);
}
void BravoGizmo::OnRotationY(const int32&)
{
	ResetInput();
	InputMask = GetUpVector_World();
	InputPlane = GetUpVector_World();

	bInputActive = CastRay(OldIntersection);
}
void BravoGizmo::OnRotationZ(const int32&)
{
	ResetInput();
	InputMask = GetForwardVector_World();
	InputPlane = GetForwardVector_World();

	bInputActive = CastRay(OldIntersection);
}

bool BravoGizmo::CastRay(glm::vec3& OutIntersection)
{
	glm::vec3 Origin, Direction;
	Engine->GetViewport()->DeProject(Engine->GetInput()->GetMousePosition(), Origin, Direction);
	float Dist = 0.0f;
	if ( glm::intersectRayPlane(Origin, Direction, GetLocation(), InputPlane, Dist) )
	{
		OutIntersection = Origin + Direction * Dist;
		return true;
	}
	if ( glm::intersectRayPlane(Origin, Direction, GetLocation(), -InputPlane, Dist) )
	{
		OutIntersection = Origin + Direction * Dist;
		return true;
	}
	return false;
}

void BravoGizmo::OnInput_MouseReleased(bool ButtonState, float DeltaTime)
{
	ResetInput();
}

void BravoGizmo::ResetInput()
{
	bInputActive = false;
	
	OldIntersection = glm::vec3(0.0);
	InputMask = glm::vec3(0.0);
	InputPlane = glm::vec3(0.0);
}


void BravoGizmo::OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime)
{
	std::shared_ptr<BravoInput> Input = Engine->GetInput();
	if ( !Input )
		return;

	if ( !bInputActive )
		return;

	glm::vec3 Intersection = glm::vec3(0.0);
	if ( !CastRay(Intersection) )
		return;

	if ( GizmoState == EBravoGizmoState::Transform )
	{
		glm::vec3 WorlDelta = (Intersection - OldIntersection) * InputMask;
		if ( glm::length(WorlDelta) >= FLT_EPS )
		{
			Log::LogMessage(ELog::Log, "Translate : {}", WorlDelta);

			for ( std::weak_ptr<IBravoTransformable>& it : Attachments )
			{
				if ( it.expired() )
					continue;
			
				std::shared_ptr<IBravoTransformable> asTransformable = it.lock();
				asTransformable->SetLocation_World(asTransformable->GetLocation_World() + WorlDelta);
			}

		
			SetLocation_World(GetLocation_World() + WorlDelta);
		}
	}
	else if ( GizmoState == EBravoGizmoState::Scale )
	{
		bool bIndividualScale = Input->GetKeyState(GLFW_KEY_LEFT_CONTROL) || Input->GetKeyState(GLFW_KEY_RIGHT_CONTROL);
				
		float OldOffset = glm::length((GetLocation_World() - OldIntersection) * InputMask);
		float CurrentOffset = glm::length((GetLocation_World() - Intersection) * InputMask);

		if ( !BravoMath::IsNearlyZero(OldOffset) && !BravoMath::IsNearlyZero(OldOffset - CurrentOffset) )
		{
			float ScaleFactor = CurrentOffset / OldOffset;

			glm::vec3 ActualScaleFactor = glm::vec3(1.0f);
			if ( bIndividualScale )
			{
				ActualScaleFactor.x = InputMask.x == 0.0f ? 1.0f : InputMask.x * ScaleFactor;
				ActualScaleFactor.y = InputMask.y == 0.0f ? 1.0f : InputMask.y * ScaleFactor;
				ActualScaleFactor.z = InputMask.z == 0.0f ? 1.0f : InputMask.z * ScaleFactor;
			}
			else
			{
				ActualScaleFactor = glm::vec3(ScaleFactor);
			}

			Log::LogMessage(ELog::Log, "Scale : {}", ActualScaleFactor);
			
			for ( std::weak_ptr<IBravoTransformable>& it : Attachments )
			{
				if ( it.expired() )
					continue;
			
				std::shared_ptr<IBravoTransformable> asTransformable = it.lock();
				asTransformable->SetScale_World(asTransformable->GetScale_World() * ActualScaleFactor);
			}
		}
	}
	else if ( GizmoState == EBravoGizmoState::Rotation )
	{
		glm::vec3 currentOffset = glm::normalize(Intersection - GetLocation());
		glm::vec3 oldOffset = glm::normalize(OldIntersection - GetLocation());

		float rad = -std::atan2(glm::dot(glm::cross(oldOffset, currentOffset), InputPlane), glm::dot(oldOffset, currentOffset));
		

		if ( glm::abs(rad) > FLT_EPS )
		{
			glm::quat rotation(rad, InputPlane);

			Log::LogMessage(ELog::Log, "Rotate : {}", rotation);

			for ( std::weak_ptr<IBravoTransformable>& it : Attachments )
			{
				if ( it.expired() )
					continue;
			
				std::shared_ptr<IBravoTransformable> asTransformable = it.lock();
				asTransformable->SetRotation_World(glm::normalize(rotation * asTransformable->GetRotation_World()));
			}

			std::vector<glm::quat> r;
			for ( auto it : TransformComponents )
				r.push_back(it->GetRotation_World());

			SetRotation_World(glm::normalize(rotation * GetRotation_World()));

			int32 i = 0;
			for ( auto it : TransformComponents )
				it->SetRotation_World(r[i++]);
		}
	}

	OldIntersection = Intersection;
}

void BravoGizmo::UpdateGizmoAttachments(std::list<std::weak_ptr<IBravoTransformable>> NewAttachments)
{
	Attachments = NewAttachments;
}