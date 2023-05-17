#include "BravoScreen_Debug.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoFont.h"
#include "BravoWidget_Text.h"

bool BravoScreen_Debug::Initialize_Internal()
{
	if ( !Engine )
		return false;

	auto font = Engine->GetAssetManager()->LoadAsset<BravoFont>("Fonts\\arial.ttf", {"50", "25", "12" });
	std::shared_ptr<BravoWidget_Text> _fps = Engine->NewObject<BravoWidget_Text>();
	_fps->SetPosition(glm::vec2(1.0f, 0.0f));
	_fps->SetOrigin(glm::vec2(1.0f, 0.0f));
	_fps->SetMargin(glm::vec2(2.0f, 2.0f));
	_fps->SetTextSize(20);
	_fps->SetText("FPS: ");
	_fps->SetFont(font);
	FPSWidget = _fps;
	AddWidget(_fps);

	return true;
}

void BravoScreen_Debug::Tick(float DeltaTime)
{
	std::shared_ptr<BravoWidget_Text> _fpsWidget = FPSWidget.expired() ? nullptr : FPSWidget.lock();

	if ( !_fpsWidget )
		return;

	FPSUpdateTimer += DeltaTime;
	if ( FPSUpdateTimer < FPSUpdateRate )
		return;
	FPSUpdateTimer = 0.0f;

	int32 FPS = (int32)(1.0f / DeltaTime);
	_fpsWidget->SetText("FPS: " + std::to_string(FPS));

	if ( FPS >= 59 )
		_fpsWidget->SetTextColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	else if ( FPS >= 29 )
		_fpsWidget->SetTextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	else
		_fpsWidget->SetTextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}
