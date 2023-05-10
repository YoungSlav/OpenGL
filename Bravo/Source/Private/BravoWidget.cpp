#include "BravoWidget.h"

void BravoWidget::Render()
{
	Render_Internal();
}

void BravoWidget::SetPosition(const glm::ivec2& _Position)
{
	ScreenPosition = _Position;
}