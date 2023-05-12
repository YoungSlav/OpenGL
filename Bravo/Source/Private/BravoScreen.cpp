#include "BravoScreen.h"
#include "BravoWidget.h"

bool BravoScreen::Initialize()
{
	return Initialize_Internal();
}

void BravoScreen::Render()
{
	for ( std::shared_ptr<BravoWidget>& w : Widgets )
		w->Render();
}