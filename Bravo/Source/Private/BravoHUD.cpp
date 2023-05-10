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
	ModelMatrix = glm::ortho(0.0f, Size.x,  Size.y, 0.0f);

	Test = std::shared_ptr<BravoWidget_Text>(new BravoWidget_Text(Self<BravoHUD>(), GetEngine()->GetAssetManager()));
	Test->Initialize();
	Test->SetPosition(glm::vec2(512.0f, 0.0f));
	Test->SetTextAlignment(EBravoTextAlignment::Center);
	Test->SetTextSize(100);
	Test->SetText("Qfj");
	Test->SetFont(GetEngine()->GetAssetManager()->LoadAsset<BravoFont>("Fonts\\arial.ttf", {"100"}));
}

void BravoHUD::SetSize(const glm::vec2& _Size)
{
	Size = _Size;
	ModelMatrix = glm::ortho(0.0f, Size.x, Size.y, 0.0f);
}


void BravoHUD::Render()
{
	Test->Render();
}

