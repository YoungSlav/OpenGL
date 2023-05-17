#include "BravoHUD.h"

#include "openGL.h"



#include <algorithm>
#include "BravoScreen.h"

bool BravoHUD::Initialize_Internal()
{
	SetSize(TargetSize);

	return true;
}

void BravoHUD::AddScreen(std::shared_ptr<BravoScreen> _Screen)
{
	Screens.push_back(_Screen);
	sort(Screens.begin(), Screens.end(), 
			[](std::shared_ptr<BravoScreen> left, std::shared_ptr<BravoScreen> right) -> bool
			{ 
				return left->GetRenderPriority() < right->GetRenderPriority(); 
			});
}

void BravoHUD::RemoveScreen(std::shared_ptr<class BravoScreen> _Screen)
{
	Screens.erase(std::remove(Screens.begin(), Screens.end(), _Screen), Screens.end());
}


void BravoHUD::SetSize(const glm::vec2& _Size)
{
	Size = _Size;
	ModelMatrix = glm::ortho(0.0f, Size.x, Size.y, 0.0f);
}

void BravoHUD::Render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for ( auto screen : Screens )
	{
		screen->Render();
	}
}