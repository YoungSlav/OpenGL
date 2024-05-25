#include "BravoWidget.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoHUD.h"
#include "BravoScreen.h"
#include "imgui/imgui.h"

bool BravoWidget::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	if ( !Engine || !Engine->GetViewport()->GetHUD() || !Engine->GetAssetManager() )
		return false;

	HUD = Engine->GetViewport()->GetHUD();
	AssetManager = Engine->GetAssetManager();

	GetHUD()->OnHUDResized.AddSP(Self<BravoWidget>(), &BravoWidget::OnHUDResized);

	Label = GetName() + "##" + std::to_string(GetHandle());

	return true;
}

void BravoWidget::Render(float DeltaTime)
{
	Render_Internal(DeltaTime);	
}