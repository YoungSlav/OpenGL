#include "BravoScreen.h"
#include "BravoEngine.h"
#include "BravoWidget.h"
#include "BravoHUD.h"

bool BravoScreen::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	if ( !Engine || !Engine->GetHUD() )
		return false;

	HUD = Engine->GetHUD();
	
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

	if ( !GetHUD() )
		return;

	for ( int32 i = (int32)Widgets.size()-1; i >= 0; --i )
	{
		Widgets[i]->Destroy();
	}

	GetHUD()->RemoveScreen(Self<BravoScreen>());
}