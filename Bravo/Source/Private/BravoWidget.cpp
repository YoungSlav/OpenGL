#include "BravoWidget.h"
#include "BravoHUD.h"

bool BravoWidget::Initialize()
{
	return Initialize_Internal();
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