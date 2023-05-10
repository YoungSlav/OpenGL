#include "BravoWidget.h"

bool BravoWidget::Initialize()
{
	return Initialize_Internal();
}

void BravoWidget::Render()
{
	Render_Internal();
}

void BravoWidget::SetPosition(const glm::ivec2& _Position)
{
	ScreenPosition = _Position;
}