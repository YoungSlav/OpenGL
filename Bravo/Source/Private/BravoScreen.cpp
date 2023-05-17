#include "BravoScreen.h"
#include "BravoEngine.h"
#include "BravoWidget.h"

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
	Widgets.push_back(_Widget);
}