#include "BravoSelectionManager.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoRenderTarget.h"
#include "BravoInput.h"
#include "IBravoRenderable.h"
#include "BravoGizmo.h"
#include "BravoStaticMeshComponent.h"

bool BravoSelectionManager::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	Size = Engine->GetViewport()->GetViewportSize();
	SelectionRenderTarget = NewObject<BravoRenderTarget>("SelectionRenderTarget");
	SelectionRenderTarget->Setup(Size, GL_RG32F, GL_RG, GL_FLOAT, true);

	return true;
}

void BravoSelectionManager::OnBeginPlay()
{
	Engine->GetViewport()->OnResizeDelegate.AddSP(Self<BravoSelectionManager>(), &BravoSelectionManager::OnViewportResized);

	if ( std::shared_ptr<BravoInput> Input = Engine->GetInput() )
	{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_MOUSE_BUTTON_LEFT;
		subscription.SubscribedType = EKeySubscriptionType::Pressed;
		subscription.Callback.BindSP(Self<BravoSelectionManager>(), &BravoSelectionManager::OnMouseClicked);
		Input->SubscribeKey(subscription);
	}
}

void BravoSelectionManager::OnViewportResized(const glm::ivec2& ViewportSize)
{
	Size = ViewportSize;
	SelectionRenderTarget->Resize(Size);
}

void BravoSelectionManager::OnDestroy()
{
	SelectionRenderTarget->Destroy();
	BravoObject::OnDestroy();
}

void BravoSelectionManager::OnMouseClicked(bool ButtonState, float DeltaTime)
{
	if ( std::shared_ptr<BravoInput> Input = Engine->GetInput() )
	{
		SelectionRenderTarget->Bind();
			
			
			Engine->GetViewport()->RenderSelectionIDs();

			glReadBuffer(GL_COLOR_ATTACHMENT0);
			
			glm::vec2 MousePosition = Input->GetMousePosition();
			GLfloat pixelColor[2];
			GLint mX = (GLint)MousePosition.x;
			GLint mY = (GLint)(Size.y - (int32)MousePosition.y);
			glReadPixels(mX, mY, 1, 1, GL_RG, GL_FLOAT, pixelColor);


			BravoSelection selection;
			BravoHandle handle = (BravoHandle)(pixelColor[0]);
			if ( auto Object = Engine->FindObjectByHandle(handle) )
			{
				selection.Object = std::dynamic_pointer_cast<IBravoRenderable>(Object);
				selection.InstanceIndex = (int32)(pixelColor[1]);
				if ( selection.Object != nullptr )
				{
					selection.Object->ObjectClicked(selection.InstanceIndex);
					ChangeSelection(selection);
				}
			}

		SelectionRenderTarget->Unbind();
	}
}

void BravoSelectionManager::ChangeSelection(const BravoSelection& Selection)
{
	// do not select gizmo
	if ( std::shared_ptr<BravoComponent> asComp = std::dynamic_pointer_cast<BravoComponent>(Selection.Object) )
	{
		if ( std::dynamic_pointer_cast<BravoGizmo>(asComp->GetOwningActor()) )
				return;
	}

	std::shared_ptr<BravoInput> Input = Engine->GetInput();
	if ( !Input )
		return;

	if ( Selection.Object->GetRenderGroup() != ERenderGroup::Main )
		return;


	bool bSelectIndividualInstance = Input->GetKeyState(GLFW_KEY_LEFT_SHIFT) || Input->GetKeyState(GLFW_KEY_RIGHT_SHIFT);
	bool bAddToSelection = ActiveSelections.empty() || Input->GetKeyState(GLFW_KEY_LEFT_CONTROL) || Input->GetKeyState(GLFW_KEY_RIGHT_CONTROL);
	
	if ( !bAddToSelection )
	{
		ClearSelections();
		ClearGizmo();
	}

	Selection.Object->ClearSelection();

	auto foundComp = ActiveSelections.find(Selection.Object);
	if ( foundComp == ActiveSelections.end() )
	{
		// object has no active selections

		std::vector<int32> InstancesToSelect;
		if ( bSelectIndividualInstance )
			InstancesToSelect.push_back(Selection.InstanceIndex);

		ActiveSelections.insert({Selection.Object, InstancesToSelect});
		Selection.Object->SetSelection(InstancesToSelect);

		SpawnGizmo();
	}
	else
	{
		// object has some active selections
		std::vector<int32>& CurentlySelectedInstanes = foundComp->second;
		if ( bSelectIndividualInstance && !CurentlySelectedInstanes.empty() )
		{
			// check if instance is already selected
			auto foundInst = std::find(CurentlySelectedInstanes.begin(), CurentlySelectedInstanes.end(), Selection.InstanceIndex);
			if ( foundInst == CurentlySelectedInstanes.end() )
			{
				// instance is not selected
				CurentlySelectedInstanes.push_back(Selection.InstanceIndex);
			}
			else
			{
				// instance is already selected
				CurentlySelectedInstanes.erase(foundInst);
			}
			Selection.Object->SetSelection(CurentlySelectedInstanes);
		}
		else
		{
			// deselect entire object
			ActiveSelections.erase(foundComp);
	
			if ( ActiveSelections.empty() )
				ClearGizmo();
		}
	}

	UpdateGizmo();
}

void BravoSelectionManager::UpdateGizmo()
{
	if ( Gizmo != nullptr )
	{
		std::list<std::weak_ptr<IBravoTransformable>> Attachments;
		
		for ( auto it : ActiveSelections )
		{
			if ( it.second.empty() )
			{
				if ( std::shared_ptr<IBravoTransformable> asTransformable = std::dynamic_pointer_cast<IBravoTransformable>(it.first) )
				{
					Attachments.push_back(asTransformable);
				}
			}
			else if ( std::shared_ptr<BravoStaticMeshComponent> asMesh = std::dynamic_pointer_cast<BravoStaticMeshComponent>(it.first) )
			{
				for ( const int32& InstIndex : it.second )
				{
					if ( std::shared_ptr<BravoStaticMeshInstance> instance = asMesh->GetInstance(InstIndex) )
					{
						Attachments.push_back(instance);
					}
				}
			}
		}

		Gizmo->UpdateGizmoAttachments(Attachments);
	}
}

void BravoSelectionManager::SpawnGizmo()
{
	if ( Gizmo != nullptr || ActiveSelections.empty() )
		return;

	auto it = ActiveSelections.begin();
	if ( std::shared_ptr<IBravoTransformable> asTransformable = std::dynamic_pointer_cast<IBravoTransformable>(it->first) )
	{
		BravoTransform spawnTransform;
		if ( it->second.empty() )
		{
			spawnTransform.SetLocation(asTransformable->GetLocation_World());
		}
		else if ( std::shared_ptr<BravoStaticMeshComponent> asMesh = std::dynamic_pointer_cast<BravoStaticMeshComponent>(asTransformable) )
		{
			if ( auto inst = asMesh->GetInstance(it->second[0]))
			{
				spawnTransform.SetLocation(inst->GetLocation_World());
			}
		}
		else
		{
			return;
		}

		Gizmo = NewObject<BravoGizmo>("Gizmo");
		Gizmo->SetTransform(spawnTransform);
	}


}
void BravoSelectionManager::ClearGizmo()
{
	if ( Gizmo != nullptr )
	{
		Gizmo->Destroy();
		Gizmo.reset();
	}
}

void BravoSelectionManager::ClearSelections()
{
	for ( auto it : ActiveSelections )
	{
		it.first->ClearSelection();
	}
	ActiveSelections.clear();
}