#include "BravoScreen.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoWidget.h"
#include "BravoHUD.h"

bool BravoScreen::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	if ( !Engine || !Engine->GetViewport()->GetHUD() )
		return false;

	HUD = Engine->GetViewport()->GetHUD();
	
	return true;
}

void BravoScreen::Render()
{
	for ( std::shared_ptr<BravoWidget>& w : Widgets )
		w->Render();
}

void BravoScreen::SetRenderPriority(int32 _RenderPriority)
{
	RenderPriority = _RenderPriority;
}

void BravoScreen::AddWidget(std::shared_ptr<class BravoWidget> _Widget)
{
	_Widget->SetOwnerScreen(Self<BravoScreen>());
	Widgets.push_back(_Widget);
}

void BravoScreen::RemoveWidget(std::shared_ptr<class BravoWidget> _Widget)
{
	Widgets.erase(std::remove(Widgets.begin(), Widgets.end(), _Widget), Widgets.end());
}

void BravoScreen::OnDestroy()
{
	BravoObject::OnDestroy();
	Widgets.clear();

	if ( !GetHUD() )
		return;

	GetHUD()->RemoveScreen(Self<BravoScreen>());
}