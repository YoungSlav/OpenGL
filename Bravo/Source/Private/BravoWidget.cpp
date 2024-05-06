#include "BravoWidget.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoHUD.h"
#include "BravoScreen.h"

bool BravoWidget::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	if ( !Engine || !Engine->GetViewport()->GetHUD() || !Engine->GetAssetManager() )
		return false;

	HUD = Engine->GetViewport()->GetHUD();
	AssetManager = Engine->GetAssetManager();

	return true;
}

void BravoWidget::Render()
{
	Render_Internal();
}

void BravoWidget::SetPosition(const glm::vec2& _Position)
{
	Position = _Position;
}

void BravoWidget::SetSize(const glm::vec2& _Size)
{
	Size = _Size;
}

void BravoWidget::SetOrigin(const glm::vec2& _Origin)
{
	Origin = _Origin;
}

void BravoWidget::SetOwnerScreen(std::shared_ptr<class BravoScreen> _OwnerScreen)
{
	OwnerScreen = _OwnerScreen;
}

glm::vec2 BravoWidget::GetActualSize() const
{
	if ( !GetHUD() )
		return Size;

	if ( bTrueScaling )
		return GetSize() * GetHUD()->GetSize();
	else
		return GetSize() * GetHUD()->GetTargetSize() * GetHUD()->GetTargetScale().y;

}

void BravoWidget::SetTrueScaling(bool _bTrueScaling)
{
	bTrueScaling = _bTrueScaling;
}

void BravoWidget::OnDestroy()
{
	BravoObject::OnDestroy();

	if( OwnerScreen.expired() )
		return;

	OwnerScreen.lock()->RemoveWidget(Self<BravoWidget>());
}