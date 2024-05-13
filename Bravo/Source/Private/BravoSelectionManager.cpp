#include "BravoSelectionManager.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoRenderTarget.h"
#include "BravoInput.h"
#include "IBravoRenderable.h"
#include "BravoGizmo.h"

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
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
					ChangeSelection(selection);
					
				}
			}

		SelectionRenderTarget->Unbind();
	}
}

void BravoSelectionManager::ChangeSelection(const BravoSelection& Selection)
{
	Selection.Object->ObjectClicked(Selection.InstanceIndex);
	OnObjectSelected.Broadcast(Selection);
	if ( std::shared_ptr<BravoComponent> asComp = std::dynamic_pointer_cast<BravoComponent>(Selection.Object) )
	{
		if ( std::dynamic_pointer_cast<BravoGizmo>(asComp->GetOwningActor()) )
			return;
		
		ClearSelection();
		Gizmo = NewObject<BravoGizmo>("Gizmo");
		Gizmo->SetLocation( asComp->GetLocation_World());
	}
	
}

void BravoSelectionManager::ClearSelection()
{
	if ( Gizmo != nullptr )
		Gizmo->Destroy();
}