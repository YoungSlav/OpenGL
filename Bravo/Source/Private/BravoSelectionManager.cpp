#include "BravoSelectionManager.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoRenderTarget.h"
#include "BravoInput.h"
#include "IBravoRenderable.h"
#include "BravoGizmo.h"
#include "BravoStaticMeshComponent.h"
#include "BravoActor.h"

bool BravoSelectionManager::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	Size = Engine->GetViewport()->GetViewportSize();
	SelectionRenderTarget = NewObject<BravoRenderTarget>("SelectionRenderTarget", Size, GL_RG32F, GL_RG, GL_FLOAT, true);

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

	SpawnGizmo();
	HideGizmo();
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
	if ( !bAllowSelections )
		return;
	std::shared_ptr<BravoInput> Input = Engine->GetInput();
	if ( !Input )
		return;
	


	SelectionRenderTarget->Bind();
			
		Engine->GetViewport()->RenderSelectionIDs();

		glReadBuffer(GL_COLOR_ATTACHMENT0);
			
		glm::vec2 MousePosition = Input->GetMousePosition();
		GLfloat pixelColor[2];
		GLint mX = (GLint)MousePosition.x;
		GLint mY = (GLint)(Size.y - (int32)MousePosition.y);
		glReadPixels(mX, mY, 1, 1, GL_RG, GL_FLOAT, pixelColor);


		BravoHandle handle = (BravoHandle)(pixelColor[0]);
		int32 InstanceIndex = (int32)(pixelColor[1]);

		const bool bSelectIndividualInstance = Input->GetKeyState(GLFW_KEY_LEFT_SHIFT) || Input->GetKeyState(GLFW_KEY_RIGHT_SHIFT);
		const bool bAddToSelection = Input->GetKeyState(GLFW_KEY_LEFT_CONTROL) || Input->GetKeyState(GLFW_KEY_RIGHT_CONTROL);
		const bool bSelectComponent = bSelectIndividualInstance || (Input->GetKeyState(GLFW_KEY_LEFT_ALT) || Input->GetKeyState(GLFW_KEY_RIGHT_ALT));

		DispatchSelection(handle, InstanceIndex, bSelectIndividualInstance, bAddToSelection, bSelectComponent);

	SelectionRenderTarget->Unbind();
}

void BravoSelectionManager::SelectObject(std::shared_ptr<BravoObject> obj, bool bAddToSelection)
{
	DispatchSelection(obj->GetHandle(), 0, false, bAddToSelection, true);
}

void BravoSelectionManager::DispatchSelection(BravoHandle handle, int32 inst, bool bSelectIndividualInstance, bool bAddToSelection, bool bSelectComponent)
{
	std::shared_ptr<BravoObject> SelectedObject = Engine->FindObjectByHandle(handle);
	if ( !SelectedObject )
	{
		ClearSelections(true);
		return;
	}
	std::shared_ptr<BravoInput> Input = Engine->GetInput();
	if ( !Input )
		return;

	// handle gizmo individually
	if ( std::shared_ptr<BravoStaticMeshComponent> asComp = std::dynamic_pointer_cast<BravoStaticMeshComponent>(SelectedObject) )
	{
		if ( std::dynamic_pointer_cast<BravoGizmo>(asComp->GetOwningActor()) )
		{
			asComp->ObjectClicked(inst);
			return;
		}
	}
	if ( std::shared_ptr<IBravoRenderable> asRenderable = std::dynamic_pointer_cast<IBravoRenderable>(SelectedObject) )
	{
		if ( asRenderable->GetRenderGroup() != ERenderGroup::Main )
		{
			return;
		}
	}

	if ( !bAddToSelection )
		ClearSelections(false);

	BravoSelection Selection;
	Selection.Object = SelectedObject;
	Selection.InstanceIndex = inst;
	
	if ( !bSelectComponent )
	{
		if ( std::shared_ptr<BravoComponent> asComp = std::dynamic_pointer_cast<BravoComponent>(SelectedObject) )
		{
			Selection.Object = asComp->GetOwningActor();
			Selection.InstanceIndex = -1;
		}
	}

	Log::LogMessage(ELog::Log, "selection : {}, {}", Selection.Object->GetName(), inst);

	AddToSelection(Selection, bSelectIndividualInstance);
}

void BravoSelectionManager::AddToSelection(const BravoSelection& Selection, bool bSelectIndividualInstance)
{
	auto foundComp = ActiveSelections.find(Selection.Object);
	if ( foundComp == ActiveSelections.end() )
	{
		// object has no active selections
		std::vector<int32> InstancesToSelect;
		if ( bSelectIndividualInstance )
			InstancesToSelect.push_back(Selection.InstanceIndex);

		ActiveSelections.insert({Selection.Object, InstancesToSelect});
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
		}
		else
		{
			// deselect entire object
			ActiveSelections.erase(foundComp);
		}
	}
	NormalizeSelections();
	UpdateHighlights();

	OnSelectionChanged.Broadcast();
}

// always keep only hightest level in selection pool.
// if component instance is selected, but owning actor is added to selection, remove instance, etc
// potentially respawn gizmo?
void BravoSelectionManager::NormalizeSelections()
{
	for (auto it = ActiveSelections.begin(); it != ActiveSelections.end(); )
	{
		bool erase = false;
		if ( it->first.expired() )
		{
			it = ActiveSelections.erase(it);
			continue;
		}

		if ( std::shared_ptr<BravoComponent> asComp = std::dynamic_pointer_cast<BravoComponent>(it->first.lock()) )
		{
			std::shared_ptr<BravoActor> owningActor = asComp->GetOwningActor();
			if ( ActiveSelections.find(owningActor) != ActiveSelections.end() )
				erase = true;
		}

		if (erase)
		{
			it = ActiveSelections.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void BravoSelectionManager::UpdateHighlights()
{
	ActiveHighlights.clear();


	for ( auto it : ActiveSelections )
	{
		if ( std::shared_ptr<BravoActor> asActor = std::dynamic_pointer_cast<BravoActor>(it.first.lock()) )
		{
			// hightlight everything
			std::vector<std::shared_ptr<BravoComponent>> components = asActor->GetComponents();
			for ( auto it : components )
			{
				if ( std::shared_ptr<IBravoRenderable> asRenderable = std::dynamic_pointer_cast<IBravoRenderable>(it) )
				{
					ActiveHighlights.insert({asRenderable, {}});
				}
			}
		}
		else if ( std::shared_ptr<IBravoRenderable> asRenderable = std::dynamic_pointer_cast<IBravoRenderable>(it.first.lock()) )
		{
			ActiveHighlights.insert({asRenderable, it.second});
		}
	}
	UpdateGizmo();
}

void BravoSelectionManager::UpdateGizmo()
{
	if ( !Gizmo )
		return;

	std::vector<std::weak_ptr<IBravoTransformable>> Attachments;
		
	for ( auto it : ActiveSelections )
	{
		if ( it.second.empty() )
		{
			if ( std::shared_ptr<IBravoTransformable> asTransformable = std::dynamic_pointer_cast<IBravoTransformable>(it.first.lock()) )
			{
				Attachments.push_back(asTransformable);
			}
		}
		else if ( std::shared_ptr<BravoStaticMeshComponent> asMesh = std::dynamic_pointer_cast<BravoStaticMeshComponent>(it.first.lock()) )
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

	Gizmo->SetVisisble(Attachments.size() != 0);
	Gizmo->UpdateGizmoAttachments(Attachments);
}

void BravoSelectionManager::SpawnGizmo()
{
	if ( Gizmo != nullptr  )
		return;

	BravoTransform spawnTransform;
	Gizmo = NewObject<BravoGizmo>("Gizmo");
	Gizmo->SetTransform(spawnTransform);
}

void BravoSelectionManager::HideGizmo()
{
	Gizmo->SetVisisble(false);
}

void BravoSelectionManager::ClearSelections(bool bBroadcastChange)
{
	HideGizmo();
	ActiveSelections.clear();
	ActiveHighlights.clear();
	if ( bBroadcastChange )
		OnSelectionChanged.Broadcast();
}