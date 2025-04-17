#include "BravoScreen_ObjectProperties.h"

#include "BravoActor.h"
#include "BravoComponent.h"
#include "BravoInput.h"
#include "BravoEngine.h"

bool BravoScreen_ObjectProperties::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() || TargetObject == nullptr )
		return false;

	SetTrueScaling(false);
	SetSize(glm::vec2(0.2f, 1.0f));
	SetMaxSize(glm::vec2(1.0f, 1.0f));
	SetOrigin(glm::vec2(1.0f, 0.0f));
	SetPosition(glm::vec2(1.0f, 0.0f));

	if ( Engine->GetInput() )
	{
		BravoKeySubscription sub;
		sub.Key = GLFW_KEY_GRAVE_ACCENT;
		sub.SubscribedType = EKeySubscriptionType::Released;
		sub.Callback.BindSP(Self<BravoScreen_ObjectProperties>(), &BravoScreen_ObjectProperties::OnToggleHUD);
		Engine->GetInput()->SubscribeKey(sub);
	}
	
	return true;
}


void BravoScreen_ObjectProperties::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}

void BravoScreen_ObjectProperties::Render_Internal(float DeltaTime)
{
	if ( !bShowHUD )
		return;

	BravoScreen::Render_Internal(DeltaTime);

}
