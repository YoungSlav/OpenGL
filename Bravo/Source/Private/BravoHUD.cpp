#include "BravoHUD.h"
#include "BravoRenderTarget.h"
#include "BravoAssetManager.h"
#include "BravoShader.h"
#include "BravoWidget.h"
#include "BravoWidget_Text.h"
#include "BravoFont.h"


void BravoHUD::Init()
{
	Size = TargetSize;
	
	ModelMatrix = glm::ortho(0.0f, Size.x, 0.0f, Size.y);

	Test = std::shared_ptr<BravoWidget_Text>(new BravoWidget_Text(Self<BravoHUD>()));
	Test->SetPosition(glm::vec2(100.0f, 100.0f));
	Test->SetTextSize(48);
	Test->SetText("The quick brown fox jumps over the lazy dog!");
	Test->SetFont(GetEngine()->GetAssetManager()->LoadAsset<BravoFont>("Fonts\\arial.ttf", {"48"}));
}

void BravoHUD::SetSize(const glm::vec2& _Size)
{
	Size = _Size;
	ModelMatrix = glm::ortho(0.0f, Size.x, 0.0f, Size.y);
}


void BravoHUD::Render()
{
	Test->Render();
}

